#include "ArduinoCompat.h"
#include "MspM0Core.h"

static bool g_core_initialized;
static bool g_gpio_powered;
static bool g_uart0_powered;
static bool g_i2c0_powered;
static bool g_spi0_powered;
static bool g_adc_powered;
static bool g_timers_powered;

static void mspm0_enable_gpio(void)
{
    if (!g_gpio_powered) {
        DL_GPIO_reset(GPIOA);
        DL_GPIO_enablePower(GPIOA);
        delay_cycles(16);
        g_gpio_powered = true;
    }
}

void mspm0_core_init(void)
{
    if (g_core_initialized) {
        return;
    }

    mspm0_enable_gpio();
    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);
    DL_CORE_configInstruction(DL_CORE_CACHE_ENABLED, DL_CORE_PREFETCH_ENABLED,
        DL_CORE_LITERAL_CACHE_ENABLED);

    DL_SYSTICK_init(MSPM0_F_CPU / 1000UL);
    DL_SYSTICK_enableInterrupt();
    DL_SYSTICK_enable();

    g_core_initialized = true;
}

void mspm0_gpio_init(void)
{
    mspm0_enable_gpio();
}

void mspm0_uart0_init(unsigned long baud, uint16_t config)
{
    static const DL_UART_Main_ClockConfig clockConfig = {
        .clockSel = DL_UART_MAIN_CLOCK_BUSCLK,
        .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1,
    };
    DL_UART_Main_Config uartConfig = {
        .mode = DL_UART_MAIN_MODE_NORMAL,
        .direction = DL_UART_MAIN_DIRECTION_TX_RX,
        .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
        .parity = DL_UART_MAIN_PARITY_NONE,
        .wordLength = DL_UART_MAIN_WORD_LENGTH_8_BITS,
        .stopBits = DL_UART_MAIN_STOP_BITS_ONE,
    };
    uint32_t divisorTimes64;
    uint32_t integerDivider;
    uint32_t fractionalDivider;

    mspm0_enable_gpio();
    if (!g_uart0_powered) {
        DL_UART_Main_reset(UART0);
        DL_UART_Main_enablePower(UART0);
        delay_cycles(16);
        g_uart0_powered = true;
    }

    switch (config & SERIAL_PARITY_MASK) {
    case SERIAL_PARITY_EVEN:
        uartConfig.parity = DL_UART_MAIN_PARITY_EVEN;
        break;
    case SERIAL_PARITY_ODD:
        uartConfig.parity = DL_UART_MAIN_PARITY_ODD;
        break;
    default:
        uartConfig.parity = DL_UART_MAIN_PARITY_NONE;
        break;
    }

    switch (config & SERIAL_STOP_BIT_MASK) {
    case SERIAL_STOP_BIT_2:
        uartConfig.stopBits = DL_UART_MAIN_STOP_BITS_TWO;
        break;
    default:
        uartConfig.stopBits = DL_UART_MAIN_STOP_BITS_ONE;
        break;
    }

    switch (config & SERIAL_DATA_MASK) {
    case SERIAL_DATA_5:
        uartConfig.wordLength = DL_UART_MAIN_WORD_LENGTH_5_BITS;
        break;
    case SERIAL_DATA_6:
        uartConfig.wordLength = DL_UART_MAIN_WORD_LENGTH_6_BITS;
        break;
    case SERIAL_DATA_7:
        uartConfig.wordLength = DL_UART_MAIN_WORD_LENGTH_7_BITS;
        break;
    default:
        uartConfig.wordLength = DL_UART_MAIN_WORD_LENGTH_8_BITS;
        break;
    }

    DL_GPIO_initPeripheralInputFunction(g_APinDescription[PIN_SERIAL_RX].pincm,
        PIN_SERIAL_RX_FUNCTION);
    DL_GPIO_initPeripheralOutputFunction(g_APinDescription[PIN_SERIAL_TX].pincm,
        PIN_SERIAL_TX_FUNCTION);

    DL_UART_Main_disable(UART0);
    DL_UART_Main_setClockConfig(UART0, &clockConfig);
    DL_UART_Main_init(UART0, &uartConfig);
    DL_UART_Main_setOversampling(UART0, DL_UART_OVERSAMPLING_RATE_16X);

    divisorTimes64 = (uint32_t) (((MSPM0_F_CPU * 4ULL) + (baud / 2UL)) / baud);
    integerDivider = divisorTimes64 / 64UL;
    fractionalDivider = divisorTimes64 % 64UL;
    DL_UART_Main_setBaudRateDivisor(UART0, integerDivider, fractionalDivider);
    DL_UART_enableFIFOs(UART0);
    DL_UART_setRXFIFOThreshold(UART0, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_setTXFIFOThreshold(UART0, DL_UART_TX_FIFO_LEVEL_EMPTY);
    DL_UART_clearInterruptStatus(UART0, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_TX);
    DL_UART_enableInterrupt(UART0, DL_UART_INTERRUPT_RX);
    DL_UART_Main_enable(UART0);
}

void mspm0_i2c0_init(uint32_t frequency)
{
    static const DL_I2C_ClockConfig clockConfig = {
        .clockSel = DL_I2C_CLOCK_BUSCLK,
        .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
    };
    uint32_t timerPeriod = 23U;

    mspm0_enable_gpio();
    if (!g_i2c0_powered) {
        DL_I2C_reset(I2C0);
        DL_I2C_enablePower(I2C0);
        delay_cycles(16);
        g_i2c0_powered = true;
    }

    if (frequency >= 400000UL) {
        timerPeriod = 5U;
    }

    DL_GPIO_initPeripheralInputFunctionFeatures(g_APinDescription[PIN_WIRE_SDA].pincm,
        PIN_WIRE_SDA_FUNCTION, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(g_APinDescription[PIN_WIRE_SCL].pincm,
        PIN_WIRE_SCL_FUNCTION, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(g_APinDescription[PIN_WIRE_SDA].pincm);
    DL_GPIO_enableHiZ(g_APinDescription[PIN_WIRE_SCL].pincm);

    DL_I2C_setClockConfig(I2C0, &clockConfig);
    DL_I2C_disableAnalogGlitchFilter(I2C0);
    DL_I2C_resetControllerTransfer(I2C0);
    DL_I2C_setTimerPeriod(I2C0, timerPeriod);
    DL_I2C_setControllerTXFIFOThreshold(I2C0, DL_I2C_TX_FIFO_LEVEL_EMPTY);
    DL_I2C_setControllerRXFIFOThreshold(I2C0, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
    DL_I2C_enableControllerClockStretching(I2C0);
    DL_I2C_enableController(I2C0);
}

void mspm0_spi0_init(void)
{
    static const DL_SPI_ClockConfig clockConfig = {
        .clockSel = DL_SPI_CLOCK_BUSCLK,
        .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1,
    };
    static const DL_SPI_Config spiConfig = {
        .mode = DL_SPI_MODE_CONTROLLER,
        .frameFormat = DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA0,
        .parity = DL_SPI_PARITY_NONE,
        .dataSize = DL_SPI_DATA_SIZE_8,
        .bitOrder = DL_SPI_BIT_ORDER_MSB_FIRST,
    };

    mspm0_enable_gpio();
    if (!g_spi0_powered) {
        DL_SPI_reset(SPI0);
        DL_SPI_enablePower(SPI0);
        delay_cycles(16);
        g_spi0_powered = true;
    }

    DL_GPIO_initPeripheralOutputFunction(g_APinDescription[PIN_SPI_SCK].pincm,
        PIN_SPI_SCK_FUNCTION);
    DL_GPIO_initPeripheralOutputFunction(g_APinDescription[PIN_SPI_MOSI].pincm,
        PIN_SPI_MOSI_FUNCTION);
    DL_GPIO_initPeripheralInputFunction(g_APinDescription[PIN_SPI_MISO].pincm,
        PIN_SPI_MISO_FUNCTION);

    DL_SPI_disable(SPI0);
    DL_SPI_setClockConfig(SPI0, &clockConfig);
    DL_SPI_init(SPI0, &spiConfig);
    DL_SPI_setFIFOThreshold(SPI0, DL_SPI_RX_FIFO_LEVEL_ONE_FRAME,
        DL_SPI_TX_FIFO_LEVEL_ONE_FRAME);
    DL_SPI_enable(SPI0);
}

void mspm0_adc_init(void)
{
    static const DL_ADC12_ClockConfig clockConfig = {
        .clockSel = DL_ADC12_CLOCK_SYSOSC,
        .divideRatio = DL_ADC12_CLOCK_DIVIDE_8,
        .freqRange = DL_ADC12_CLOCK_FREQ_RANGE_20_TO_24,
    };

    if (!g_adc_powered) {
        DL_ADC12_reset(ADC0);
        DL_ADC12_enablePower(ADC0);
        delay_cycles(16);
        DL_ADC12_setClockConfig(ADC0, &clockConfig);
        DL_ADC12_initSingleSample(ADC0, DL_ADC12_REPEAT_MODE_DISABLED,
            DL_ADC12_SAMPLING_SOURCE_AUTO, DL_ADC12_TRIG_SRC_SOFTWARE,
            DL_ADC12_SAMP_CONV_RES_12_BIT,
            DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
        DL_ADC12_setPowerDownMode(ADC0, DL_ADC12_POWER_DOWN_MODE_MANUAL);
        DL_ADC12_setSampleTime0(ADC0, 75);
        DL_ADC12_enableConversions(ADC0);
        g_adc_powered = true;
    }
}

void mspm0_pwm_init(void)
{
    if (!g_timers_powered) {
        DL_TimerG_reset(TIMG8);
        DL_TimerG_reset(TIMG14);
        DL_TimerA_reset(TIMA0);
        DL_TimerG_enablePower(TIMG8);
        DL_TimerG_enablePower(TIMG14);
        DL_TimerA_enablePower(TIMA0);
        delay_cycles(16);
        g_timers_powered = true;
    }
}
