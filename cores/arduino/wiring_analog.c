#include "ArduinoCompat.h"
#include "MspM0Core.h"
#include <ti/driverlib/dl_vref.h>

static uint8_t s_adc_resolution = 10U;
static uint8_t s_pwm_resolution = 8U;
static uint8_t s_analog_reference = AR_DEFAULT;
static bool s_vref_initialized;

static uint8_t pin_to_adc_channel(pin_size_t pin)
{
    if (pin >= PINS_COUNT) {
        return INVALID_ANALOG_CHANNEL;
    }
    return g_APinDescription[pin].adcChannel;
}

static uint32_t adc_input_for_channel(uint8_t channel)
{
    switch (channel) {
    case 0: return DL_ADC12_INPUT_CHAN_0;
    case 1: return DL_ADC12_INPUT_CHAN_1;
    case 2: return DL_ADC12_INPUT_CHAN_2;
    case 3: return DL_ADC12_INPUT_CHAN_3;
    case 4: return DL_ADC12_INPUT_CHAN_4;
    case 5: return DL_ADC12_INPUT_CHAN_5;
    case 6: return DL_ADC12_INPUT_CHAN_6;
    case 7: return DL_ADC12_INPUT_CHAN_7;
    case 8: return DL_ADC12_INPUT_CHAN_8;
    case 9: return DL_ADC12_INPUT_CHAN_9;
    default: return DL_ADC12_INPUT_CHAN_0;
    }
}

static uint32_t scale_resolution(uint32_t value, uint8_t fromBits, uint8_t toBits)
{
    if (fromBits == toBits) {
        return value;
    }
    if (toBits < fromBits) {
        return value >> (fromBits - toBits);
    }
    return value << (toBits - fromBits);
}

static uint32_t adc_reference_for_mode(uint8_t mode)
{
    if ((mode == AR_INTERNAL_1V4) || (mode == AR_INTERNAL_2V5)) {
        return DL_ADC12_REFERENCE_VOLTAGE_INTREF;
    }
    return DL_ADC12_REFERENCE_VOLTAGE_VDDA;
}

static void configure_vref(uint8_t mode)
{
#ifdef __MSPM0_HAS_VREF__
    static const DL_VREF_ClockConfig clockConfig = {
        .clockSel = DL_VREF_CLOCK_BUSCLK,
        .divideRatio = DL_VREF_CLOCK_DIVIDE_1,
    };
    DL_VREF_Config referenceConfig = {
        .vrefEnable = DL_VREF_ENABLE_ENABLE,
        .bufConfig = DL_VREF_BUFCONFIG_OUTPUT_2_5V,
        .shModeEnable = DL_VREF_SHMODE_DISABLE,
        .holdCycleCount = DL_VREF_HOLD_MIN,
        .shCycleCount = DL_VREF_SH_MIN,
    };

    if ((mode != AR_INTERNAL_1V4) && (mode != AR_INTERNAL_2V5)) {
        if (s_vref_initialized) {
            DL_VREF_disableInternalRef(VREF);
        }
        return;
    }

    if (!s_vref_initialized) {
        DL_VREF_reset(VREF);
        DL_VREF_enablePower(VREF);
        delay_cycles(16);
        DL_VREF_setClockConfig(VREF, &clockConfig);
        s_vref_initialized = true;
    }

    if (mode == AR_INTERNAL_1V4) {
        referenceConfig.bufConfig = DL_VREF_BUFCONFIG_OUTPUT_1_4V;
    } else {
        referenceConfig.bufConfig = DL_VREF_BUFCONFIG_OUTPUT_2_5V;
    }

    DL_VREF_configReference(VREF, &referenceConfig);
#else
    (void) mode;
#endif
}

static void init_pwm_timer_g8(void)
{
    static bool initialized;
    static const DL_TimerG_ClockConfig clockConfig = {
        .clockSel = DL_TIMER_CLOCK_BUSCLK,
        .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
        .prescale = 255U,
    };
    static const DL_TimerG_PWMConfig pwmConfig = {
        .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
        .period = 255U,
        .isTimerWithFourCC = true,
        .startTimer = DL_TIMER_STOP,
    };

    if (initialized) {
        return;
    }

    mspm0_pwm_init();
    DL_TimerG_setClockConfig(TIMG8, &clockConfig);
    DL_TimerG_initPWMMode(TIMG8, &pwmConfig);
    DL_TimerG_setCounterControl(TIMG8, DL_TIMER_CZC_CCCTL0_ZCOND,
        DL_TIMER_CAC_CCCTL0_ACOND, DL_TIMER_CLC_CCCTL0_LCOND);
    DL_TimerG_setCaptureCompareOutCtl(TIMG8, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareOutCtl(TIMG8, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptCompUpdateMethod(TIMG8, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptCompUpdateMethod(TIMG8, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_enableClock(TIMG8);
    DL_TimerG_setCCPDirection(TIMG8, DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT);
    DL_TimerG_startCounter(TIMG8);
    initialized = true;
}

#if MSPM0_HAS_PWM_TIMG14
static void init_pwm_timer_g14(void)
{
    static bool initialized;
    static const DL_TimerG_ClockConfig clockConfig = {
        .clockSel = DL_TIMER_CLOCK_BUSCLK,
        .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
        .prescale = 255U,
    };
    static const DL_TimerG_PWMConfig pwmConfig = {
        .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
        .period = 255U,
        .isTimerWithFourCC = true,
        .startTimer = DL_TIMER_STOP,
    };

    if (initialized) {
        return;
    }

    mspm0_pwm_init();
    DL_TimerG_setClockConfig(TIMG14, &clockConfig);
    DL_TimerG_initPWMMode(TIMG14, &pwmConfig);
    DL_TimerG_setCounterControl(TIMG14, DL_TIMER_CZC_CCCTL0_ZCOND,
        DL_TIMER_CAC_CCCTL0_ACOND, DL_TIMER_CLC_CCCTL0_LCOND);
    DL_TimerG_setCaptureCompareOutCtl(TIMG14, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptureCompareOutCtl(TIMG14, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptureCompareOutCtl(TIMG14, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERG_CAPTURE_COMPARE_2_INDEX);
    DL_TimerG_setCaptureCompareOutCtl(TIMG14, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERG_CAPTURE_COMPARE_3_INDEX);
    DL_TimerG_setCaptCompUpdateMethod(TIMG14, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
    DL_TimerG_setCaptCompUpdateMethod(TIMG14, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERG_CAPTURE_COMPARE_1_INDEX);
    DL_TimerG_setCaptCompUpdateMethod(TIMG14, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERG_CAPTURE_COMPARE_2_INDEX);
    DL_TimerG_setCaptCompUpdateMethod(TIMG14, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE,
        DL_TIMERG_CAPTURE_COMPARE_3_INDEX);
    DL_TimerG_enableClock(TIMG14);
    DL_TimerG_setCCPDirection(TIMG14,
        DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT | DL_TIMER_CC2_OUTPUT | DL_TIMER_CC3_OUTPUT);
    DL_TimerG_startCounter(TIMG14);
    initialized = true;
}
#endif

static void init_pwm_timer_a0(void)
{
    static bool initialized;
    static const DL_TimerA_ClockConfig clockConfig = {
        .clockSel = DL_TIMER_CLOCK_BUSCLK,
        .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
        .prescale = 255U,
    };
    static const DL_TimerA_PWMConfig pwmConfig = {
        .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN,
        .period = 255U,
        .isTimerWithFourCC = true,
        .startTimer = DL_TIMER_STOP,
    };

    if (initialized) {
        return;
    }

    mspm0_pwm_init();
    DL_TimerA_setClockConfig(TIMA0, &clockConfig);
    DL_TimerA_initPWMMode(TIMA0, &pwmConfig);
    DL_TimerA_setCounterControl(TIMA0, DL_TIMER_CZC_CCCTL0_ZCOND,
        DL_TIMER_CAC_CCCTL0_ACOND, DL_TIMER_CLC_CCCTL0_LCOND);
    DL_TimerA_setCaptureCompareOutCtl(TIMA0, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareOutCtl(TIMA0, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptureCompareOutCtl(TIMA0, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptureCompareOutCtl(TIMA0, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
        DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
        DL_TIMERA_CAPTURE_COMPARE_3_INDEX);
    DL_TimerA_setCaptCompUpdateMethod(TIMA0, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT,
        DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptCompUpdateMethod(TIMA0, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT,
        DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptCompUpdateMethod(TIMA0, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT,
        DL_TIMERA_CAPTURE_COMPARE_2_INDEX);
    DL_TimerA_setCaptCompUpdateMethod(TIMA0, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT,
        DL_TIMERA_CAPTURE_COMPARE_3_INDEX);
    DL_TimerA_enableClock(TIMA0);
    DL_TimerA_enableShadowFeatures(TIMA0);
    DL_TimerA_setCCPDirection(TIMA0,
        DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT | DL_TIMER_CC2_OUTPUT | DL_TIMER_CC3_OUTPUT);
    DL_TimerA_startCounter(TIMA0);
    initialized = true;
}

static void set_pwm_disabled(const PinDescription *desc, int value)
{
    if (value <= 0) {
        DL_GPIO_clearPins(desc->gpio, desc->pinMask);
    } else {
        DL_GPIO_setPins(desc->gpio, desc->pinMask);
    }
    /* Preload the GPIO output latch before switching away from the timer
       function to avoid a visible glitch at 0%/100% duty transitions. */
    DL_GPIO_initDigitalOutput(desc->pincm);
    if (value <= 0) {
        DL_GPIO_clearPins(desc->gpio, desc->pinMask);
    } else {
        DL_GPIO_setPins(desc->gpio, desc->pinMask);
    }
}

int analogRead(pin_size_t pin)
{
    uint16_t result;
    uint8_t channel = pin_to_adc_channel(pin);

    if (channel == INVALID_ANALOG_CHANNEL) {
        return 0;
    }

    mspm0_adc_init();
    configure_vref(s_analog_reference);
    DL_GPIO_setAnalogInternalResistor(g_APinDescription[pin].pincm, DL_GPIO_RESISTOR_NONE);
    DL_ADC12_configConversionMem(MSPM0_ADC_INST, DL_ADC12_MEM_IDX_0, adc_input_for_channel(channel),
        adc_reference_for_mode(s_analog_reference), DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0,
        DL_ADC12_AVERAGING_MODE_DISABLED, DL_ADC12_BURN_OUT_SOURCE_DISABLED,
        DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_clearInterruptStatus(MSPM0_ADC_INST, DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED);
    DL_ADC12_startConversion(MSPM0_ADC_INST);
    while ((DL_ADC12_getRawInterruptStatus(MSPM0_ADC_INST, DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED) &
           DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED) == 0U) {
    }
    result = DL_ADC12_getMemResult(MSPM0_ADC_INST, DL_ADC12_MEM_IDX_0);
    return (int) scale_resolution(result, 12U, s_adc_resolution);
}

void analogReference(uint8_t mode)
{
    switch (mode) {
    case AR_DEFAULT:
    case AR_INTERNAL_1V4:
    case AR_INTERNAL_2V5:
        s_analog_reference = mode;
        configure_vref(mode);
        break;
    default:
        break;
    }
}

void analogReadResolution(int bits)
{
    if (bits < 1) {
        bits = 1;
    } else if (bits > 12) {
        bits = 12;
    }
    s_adc_resolution = (uint8_t) bits;
}

void analogWriteResolution(int bits)
{
    if (bits < 1) {
        bits = 1;
    } else if (bits > 16) {
        bits = 16;
    }
    s_pwm_resolution = (uint8_t) bits;
}

void analogWrite(pin_size_t pin, int value)
{
    const PinDescription *desc;
    uint32_t duty;
    uint32_t scaledValue;

    if ((pin >= PINS_COUNT) || !digitalPinHasPWM(pin)) {
        if (value <= 0) {
            digitalWrite(pin, LOW);
        } else {
            digitalWrite(pin, HIGH);
        }
        return;
    }

    desc = &g_APinDescription[pin];
    if (value <= 0) {
        scaledValue = 0U;
    } else {
        uint32_t maxInput = (s_pwm_resolution >= 31U) ? 0xFFFFFFFFu : ((1UL << s_pwm_resolution) - 1UL);
        scaledValue = (uint32_t) constrain(value, 0, (int) maxInput);
    }
    duty = scale_resolution(scaledValue, s_pwm_resolution, 8U);
    if (duty > 255U) {
        duty = 255U;
    }

    if ((duty == 0U) || (duty == 255U)) {
        set_pwm_disabled(desc, (int) duty);
        return;
    }

    if (desc->pwmTimer == PWM_TIMER_TIMG8) {
        init_pwm_timer_g8();
        DL_GPIO_initPeripheralOutputFunction(desc->pincm, desc->pwmFunction);
        DL_TimerG_setCaptureCompareValue(TIMG8, duty, (DL_TIMER_CC_INDEX) desc->pwmChannel);
    } else if (desc->pwmTimer == PWM_TIMER_TIMA0) {
        init_pwm_timer_a0();
        DL_GPIO_initPeripheralOutputFunction(desc->pincm, desc->pwmFunction);
        DL_TimerA_setCaptureCompareValue(TIMA0, duty, (DL_TIMER_CC_INDEX) desc->pwmChannel);
    }
#if MSPM0_HAS_PWM_TIMG14
    else if (desc->pwmTimer == PWM_TIMER_TIMG14) {
        init_pwm_timer_g14();
        DL_GPIO_initPeripheralOutputFunction(desc->pincm, desc->pwmFunction);
        DL_TimerG_setCaptureCompareValue(TIMG14, duty, (DL_TIMER_CC_INDEX) desc->pwmChannel);
    }
#endif
}
