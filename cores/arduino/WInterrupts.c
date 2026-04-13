#include "ArduinoCompat.h"
#include "MspM0Core.h"

static voidFuncPtr s_callbacks[PINS_COUNT];
static voidFuncPtrParam s_param_callbacks[PINS_COUNT];
static void *s_params[PINS_COUNT];

static void update_polarity(const PinDescription *desc, PinStatus mode)
{
    GPIO_Regs *gpio = desc->gpio;

    if (desc->pinIndex < 16U) {
        gpio->POLARITY15_0 &= ~(desc->interruptEdgeDisable | desc->interruptEdgeRise |
            desc->interruptEdgeFall | desc->interruptEdgeBoth);
        switch (mode) {
        case RISING:
            gpio->POLARITY15_0 |= desc->interruptEdgeRise;
            break;
        case FALLING:
            gpio->POLARITY15_0 |= desc->interruptEdgeFall;
            break;
        case CHANGE:
        default:
            gpio->POLARITY15_0 |= desc->interruptEdgeBoth;
            break;
        }
    } else {
        gpio->POLARITY31_16 &= ~(desc->interruptEdgeDisable | desc->interruptEdgeRise |
            desc->interruptEdgeFall | desc->interruptEdgeBoth);
        switch (mode) {
        case RISING:
            gpio->POLARITY31_16 |= desc->interruptEdgeRise;
            break;
        case FALLING:
            gpio->POLARITY31_16 |= desc->interruptEdgeFall;
            break;
        case CHANGE:
        default:
            gpio->POLARITY31_16 |= desc->interruptEdgeBoth;
            break;
        }
    }
}

static void __attribute__((unused)) dispatch_port_interrupts(GPIO_Regs *gpio)
{
    while (1) {
        DL_GPIO_IIDX pending = DL_GPIO_getPendingInterrupt(gpio);
        uint8_t pin;

        if (pending == DL_GPIO_IIDX_NO_INTR) {
            break;
        }

        for (pin = 0; pin < PINS_COUNT; pin++) {
            const PinDescription *desc = &g_APinDescription[pin];

            if ((desc->gpio == gpio) &&
                ((uint32_t) pending == ((uint32_t) desc->pinIndex + 1U))) {
                DL_GPIO_clearInterruptStatus(gpio, desc->pinMask);
                if (s_callbacks[pin] != 0) {
                    s_callbacks[pin]();
                } else if (s_param_callbacks[pin] != 0) {
                    s_param_callbacks[pin](s_params[pin]);
                }
                break;
            }
        }
    }
}

void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, PinStatus mode)
{
    const PinDescription *desc;

    if (interruptNumber >= PINS_COUNT) {
        return;
    }
    desc = &g_APinDescription[interruptNumber];
    s_callbacks[interruptNumber] = callback;
    s_param_callbacks[interruptNumber] = 0;
    s_params[interruptNumber] = 0;
    update_polarity(desc, mode);
    DL_GPIO_clearInterruptStatus(desc->gpio, desc->pinMask);
    DL_GPIO_enableInterrupt(desc->gpio, desc->pinMask);
    NVIC_ClearPendingIRQ(MSPM0_GPIO_INT_IRQN);
    NVIC_EnableIRQ(MSPM0_GPIO_INT_IRQN);
}

void attachInterruptParam(pin_size_t interruptNumber, voidFuncPtrParam callback, PinStatus mode, void* param)
{
    const PinDescription *desc;

    if (interruptNumber >= PINS_COUNT) {
        return;
    }
    desc = &g_APinDescription[interruptNumber];
    s_callbacks[interruptNumber] = 0;
    s_param_callbacks[interruptNumber] = callback;
    s_params[interruptNumber] = param;
    update_polarity(desc, mode);
    DL_GPIO_clearInterruptStatus(desc->gpio, desc->pinMask);
    DL_GPIO_enableInterrupt(desc->gpio, desc->pinMask);
    NVIC_ClearPendingIRQ(MSPM0_GPIO_INT_IRQN);
    NVIC_EnableIRQ(MSPM0_GPIO_INT_IRQN);
}

void detachInterrupt(pin_size_t interruptNumber)
{
    if (interruptNumber >= PINS_COUNT) {
        return;
    }
    DL_GPIO_disableInterrupt(g_APinDescription[interruptNumber].gpio, g_APinDescription[interruptNumber].pinMask);
    s_callbacks[interruptNumber] = 0;
    s_param_callbacks[interruptNumber] = 0;
    s_params[interruptNumber] = 0;
}

void MSPM0_GPIO_IRQ_HANDLER(void)
{
#if MSPM0_GPIO_INTERRUPTS_GROUPED
    while (1) {
        switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1)) {
#ifdef GPIOA
        case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
            dispatch_port_interrupts(GPIOA);
            break;
#endif
#ifdef GPIOB
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
            dispatch_port_interrupts(GPIOB);
            break;
#endif
#ifdef GPIOC
        case DL_INTERRUPT_GROUP1_IIDX_GPIOC:
            dispatch_port_interrupts(GPIOC);
            break;
#endif
        default:
            return;
        }
    }
#else
    dispatch_port_interrupts(GPIOA);
#endif
}
