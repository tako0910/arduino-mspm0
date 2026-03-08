#include "ArduinoCompat.h"
#include "MspM0Core.h"

static bool is_valid_pin(pin_size_t pin)
{
    return pin < PINS_COUNT;
}

void pinMode(pin_size_t pin, PinMode mode)
{
    const PinDescription *desc;

    if (!is_valid_pin(pin)) {
        return;
    }

    mspm0_gpio_init();
    desc = &g_APinDescription[pin];

    switch (mode) {
    case OUTPUT:
        DL_GPIO_initDigitalOutput(desc->pincm);
        DL_GPIO_enableOutput(desc->gpio, desc->pinMask);
        break;
    case OUTPUT_OPENDRAIN:
        DL_GPIO_initDigitalOutputFeatures(desc->pincm, DL_GPIO_INVERSION_DISABLE,
            DL_GPIO_RESISTOR_NONE, DL_GPIO_DRIVE_STRENGTH_LOW, DL_GPIO_HIZ_ENABLE);
        DL_GPIO_enableOutput(desc->gpio, desc->pinMask);
        break;
    case INPUT_PULLUP:
        DL_GPIO_initDigitalInputFeatures(desc->pincm, DL_GPIO_INVERSION_DISABLE,
            DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
            DL_GPIO_WAKEUP_DISABLE);
        break;
    case INPUT_PULLDOWN:
        DL_GPIO_initDigitalInputFeatures(desc->pincm, DL_GPIO_INVERSION_DISABLE,
            DL_GPIO_RESISTOR_PULL_DOWN, DL_GPIO_HYSTERESIS_DISABLE,
            DL_GPIO_WAKEUP_DISABLE);
        break;
    case INPUT:
    default:
        DL_GPIO_initDigitalInputFeatures(desc->pincm, DL_GPIO_INVERSION_DISABLE,
            DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
            DL_GPIO_WAKEUP_DISABLE);
        break;
    }
}

void digitalWrite(pin_size_t pin, PinStatus status)
{
    const PinDescription *desc;

    if (!is_valid_pin(pin)) {
        return;
    }

    desc = &g_APinDescription[pin];
    if (status == LOW) {
        DL_GPIO_clearPins(desc->gpio, desc->pinMask);
    } else {
        DL_GPIO_setPins(desc->gpio, desc->pinMask);
    }
}

PinStatus digitalRead(pin_size_t pin)
{
    const PinDescription *desc;

    if (!is_valid_pin(pin)) {
        return LOW;
    }

    desc = &g_APinDescription[pin];
    return (DL_GPIO_readPins(desc->gpio, desc->pinMask) != 0U) ? HIGH : LOW;
}
