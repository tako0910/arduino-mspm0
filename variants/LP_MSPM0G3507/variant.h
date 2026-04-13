#pragma once

#include <stdbool.h>
#include <stdint.h>

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

#define PINS_COUNT           (17u)
#define NUM_DIGITAL_PINS     (17u)
#define NUM_ANALOG_INPUTS    (2u)

#define PIN_SERIAL_RX        (0u)
#define PIN_SERIAL_TX        (1u)
#define PIN_WIRE_SCL         (9u)
#define PIN_WIRE_SDA         (10u)
#define PIN_SPI_SS           (11u)
#define PIN_SPI_MISO         (12u)
#define PIN_SPI_MOSI         (13u)
#define PIN_SPI_SCK          (14u)
#define LED_BUILTIN          (3u)
#define PIN_LED              LED_BUILTIN

#define PIN_SERIAL_RX_FUNCTION  IOMUX_PINCM22_PF_UART0_RX
#define PIN_SERIAL_TX_FUNCTION  IOMUX_PINCM21_PF_UART0_TX

#define MSPM0_SERIAL_INST       UART0
#define MSPM0_SERIAL_INT_IRQN   UART0_INT_IRQn
#define MSPM0_SERIAL_IRQHandler UART0_IRQHandler

#define PIN_WIRE_SDA_FUNCTION   IOMUX_PINCM16_PF_I2C1_SDA
#define PIN_WIRE_SCL_FUNCTION   IOMUX_PINCM15_PF_I2C1_SCL

#define MSPM0_WIRE_INST         I2C1

#define PIN_SPI_SCK_FUNCTION    IOMUX_PINCM26_PF_SPI1_SCLK
#define PIN_SPI_MOSI_FUNCTION   IOMUX_PINCM25_PF_SPI1_PICO
#define PIN_SPI_MISO_FUNCTION   IOMUX_PINCM24_PF_SPI1_POCI

#define MSPM0_SPI_INST          SPI1
#define MSPM0_ADC_INST          ADC0

#define MSPM0_GPIO_INTERRUPTS_GROUPED 1
#define MSPM0_GPIO_INT_IRQN     GPIOA_INT_IRQn
#define MSPM0_GPIO_IRQ_HANDLER  GROUP1_IRQHandler

#define MSPM0_HAS_PWM_TIMG14    0

#define PIN_A0               (15u)  /* PA25, ADC0 channel 2 */
#define PIN_A1               (16u)  /* PB25, ADC0 channel 4 */

static inline uint8_t analogInputToDigitalPin(uint8_t analogPin)
{
    switch (analogPin) {
    case 0: return PIN_A0;
    case 1: return PIN_A1;
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
