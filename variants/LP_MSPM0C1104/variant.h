#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PWM_TIMER_NONE = 0,
    PWM_TIMER_TIMG8 = 1,
    PWM_TIMER_TIMA0 = 2,
    PWM_TIMER_TIMG14 = 3,
} pwm_timer_t;

typedef struct {
    GPIO_Regs *gpio;
    uint32_t pinMask;
    uint8_t pinIndex;
    uint32_t pincm;
    uint8_t adcChannel;
    pwm_timer_t pwmTimer;
    uint8_t pwmChannel;
    uint32_t pwmFunction;
    uint32_t interruptEdgeDisable;
    uint32_t interruptEdgeRise;
    uint32_t interruptEdgeFall;
    uint32_t interruptEdgeBoth;
} PinDescription;

extern const PinDescription g_APinDescription[];

#define PINS_COUNT           (15u)
#define NUM_DIGITAL_PINS     (15u)
#define NUM_ANALOG_INPUTS    (9u)

#define PIN_SERIAL_RX        (0u)
#define PIN_SERIAL_TX        (1u)
#define PIN_WIRE_SCL         (6u)
#define PIN_WIRE_SDA         (7u)
#define PIN_SPI_SS           (9u)
#define PIN_SPI_MISO         (10u)
#define PIN_SPI_MOSI         (11u)
#define PIN_SPI_SCK          (4u)
#define LED_BUILTIN          (5u)
#define PIN_LED              LED_BUILTIN

#define PIN_SERIAL_RX_FUNCTION  IOMUX_PINCM27_PF_UART0_RX
#define PIN_SERIAL_TX_FUNCTION  IOMUX_PINCM28_PF_UART0_TX

#define PIN_WIRE_SDA_FUNCTION   IOMUX_PINCM1_PF_I2C0_SDA
#define PIN_WIRE_SCL_FUNCTION   IOMUX_PINCM12_PF_I2C0_SCL

#define PIN_SPI_SCK_FUNCTION    IOMUX_PINCM7_PF_SPI0_SCLK
#define PIN_SPI_MOSI_FUNCTION   IOMUX_PINCM19_PF_SPI0_PICO
#define PIN_SPI_MISO_FUNCTION   IOMUX_PINCM5_PF_SPI0_POCI

#define PIN_A0               (1u)   /* PA27 */
#define PIN_A1               (0u)   /* PA26 */
#define PIN_A2               (14u)  /* PA25 */
#define PIN_A3               (2u)   /* PA24 */
#define PIN_A4               (5u)   /* PA22 */
#define PIN_A5               (3u)   /* PA28 */
#define PIN_A7               (11u)  /* PA18 */
#define PIN_A8               (13u)  /* PA16 */
#define PIN_A9               (12u)  /* PA17 */

static inline uint8_t analogInputToDigitalPin(uint8_t analogPin)
{
    switch (analogPin) {
    case 0: return PIN_A0;
    case 1: return PIN_A1;
    case 2: return PIN_A2;
    case 3: return PIN_A3;
    case 4: return PIN_A4;
    case 5: return PIN_A5;
    case 6: return PIN_A7;
    case 7: return PIN_A8;
    case 8: return PIN_A9;
    default: return 0xFFu;
    }
}

static inline bool digitalPinHasPWM(uint8_t pin)
{
    return (pin < PINS_COUNT) && (g_APinDescription[pin].pwmTimer != PWM_TIMER_NONE);
}

static inline int32_t digitalPinToInterrupt(uint8_t pin)
{
    return (pin < PINS_COUNT) ? (int32_t) pin : -1;
}

static inline uint8_t digitalPinToPort(uint8_t pin)
{
    return pin;
}

static inline volatile uint32_t *portOutputRegister(uint8_t port)
{
    return (port < PINS_COUNT) ? &g_APinDescription[port].gpio->DOUT31_0 : 0;
}

static inline volatile uint32_t *portInputRegister(uint8_t port)
{
    return (port < PINS_COUNT) ? (volatile uint32_t *) &g_APinDescription[port].gpio->DIN31_0 : 0;
}

static inline uint32_t digitalPinToBitMask(uint8_t pin)
{
    return (pin < PINS_COUNT) ? g_APinDescription[pin].pinMask : 0U;
}

void initVariant(void);

#ifdef __cplusplus
}
#endif
