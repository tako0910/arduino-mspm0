#include "ArduinoCompat.h"
#include "MspM0Core.h"

static voidFuncPtr s_callbacks[PINS_COUNT];
static voidFuncPtrParam s_param_callbacks[PINS_COUNT];
static void *s_params[PINS_COUNT];

static void update_polarity(const PinDescription *desc, PinStatus mode)
{
    if (desc->pinIndex < 16U) {
        GPIOA->POLARITY15_0 &= ~(desc->interruptEdgeDisable | desc->interruptEdgeRise |
            desc->interruptEdgeFall | desc->interruptEdgeBoth);
        switch (mode) {
        case RISING:
            GPIOA->POLARITY15_0 |= desc->interruptEdgeRise;
            break;
        case FALLING:
            GPIOA->POLARITY15_0 |= desc->interruptEdgeFall;
            break;
        case CHANGE:
        default:
            GPIOA->POLARITY15_0 |= desc->interruptEdgeBoth;
            break;
        }
    } else {
        GPIOA->POLARITY31_16 &= ~(desc->interruptEdgeDisable | desc->interruptEdgeRise |
            desc->interruptEdgeFall | desc->interruptEdgeBoth);
        switch (mode) {
        case RISING:
            GPIOA->POLARITY31_16 |= desc->interruptEdgeRise;
            break;
        case FALLING:
            GPIOA->POLARITY31_16 |= desc->interruptEdgeFall;
            break;
        case CHANGE:
        default:
            GPIOA->POLARITY31_16 |= desc->interruptEdgeBoth;
            break;
        }
    }
}

void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, PinStatus mode)
{
    if (interruptNumber >= PINS_COUNT) {
        return;
    }
    s_callbacks[interruptNumber] = callback;
    s_param_callbacks[interruptNumber] = 0;
    s_params[interruptNumber] = 0;
    update_polarity(&g_APinDescription[interruptNumber], mode);
    DL_GPIO_clearInterruptStatus(GPIOA, g_APinDescription[interruptNumber].pinMask);
    DL_GPIO_enableInterrupt(GPIOA, g_APinDescription[interruptNumber].pinMask);
    NVIC_ClearPendingIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

void attachInterruptParam(pin_size_t interruptNumber, voidFuncPtrParam callback, PinStatus mode, void* param)
{
    if (interruptNumber >= PINS_COUNT) {
        return;
    }
    s_callbacks[interruptNumber] = 0;
    s_param_callbacks[interruptNumber] = callback;
    s_params[interruptNumber] = param;
    update_polarity(&g_APinDescription[interruptNumber], mode);
    DL_GPIO_clearInterruptStatus(GPIOA, g_APinDescription[interruptNumber].pinMask);
    DL_GPIO_enableInterrupt(GPIOA, g_APinDescription[interruptNumber].pinMask);
    NVIC_ClearPendingIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

void detachInterrupt(pin_size_t interruptNumber)
{
    if (interruptNumber >= PINS_COUNT) {
        return;
    }
    DL_GPIO_disableInterrupt(GPIOA, g_APinDescription[interruptNumber].pinMask);
    s_callbacks[interruptNumber] = 0;
    s_param_callbacks[interruptNumber] = 0;
    s_params[interruptNumber] = 0;
}

void GPIOA_IRQHandler(void)
{
    while (1) {
        DL_GPIO_IIDX pending = DL_GPIO_getPendingInterrupt(GPIOA);
        uint8_t pin;

        if (pending == DL_GPIO_IIDX_NO_INTR) {
            break;
        }

        for (pin = 0; pin < PINS_COUNT; pin++) {
            if ((uint32_t) pending == ((uint32_t) g_APinDescription[pin].pinIndex + 1U)) {
                DL_GPIO_clearInterruptStatus(GPIOA, g_APinDescription[pin].pinMask);
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
