#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#include "variant.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_ANALOG_CHANNEL 0xFFu

void mspm0_core_init(void);
void mspm0_gpio_init(void);
void mspm0_uart0_init(unsigned long baud, uint16_t config);
void mspm0_i2c0_init(uint32_t frequency);
void mspm0_spi0_init(void);
void mspm0_adc_init(void);
void mspm0_pwm_init(void);

#ifdef __cplusplus
}
#endif
