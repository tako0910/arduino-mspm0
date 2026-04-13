#pragma once

#include "variant.h"

#define PIN_D0   (0u)   /* PA11 */
#define PIN_D1   (1u)   /* PA10 */
#define PIN_D2   (2u)   /* PA18 */
#define PIN_D3   (3u)   /* PA0 */
#define PIN_D4   (4u)   /* PA1 */
#define PIN_D5   (5u)   /* PB21 */
#define PIN_D6   (6u)   /* PB22 */
#define PIN_D7   (7u)   /* PB26 */
#define PIN_D8   (8u)   /* PB27 */
#define PIN_D9   (9u)   /* PB2 */
#define PIN_D10  (10u)  /* PB3 */
#define PIN_D11  (11u)  /* PB6 */
#define PIN_D12  (12u)  /* PB7 */
#define PIN_D13  (13u)  /* PB8 */
#define PIN_D14  (14u)  /* PB9 */
#define PIN_D15  (15u)  /* PA25 */
#define PIN_D16  (16u)  /* PB25 */

#ifdef __cplusplus
static const uint8_t D0 = PIN_D0;
static const uint8_t D1 = PIN_D1;
static const uint8_t D2 = PIN_D2;
static const uint8_t D3 = PIN_D3;
static const uint8_t D4 = PIN_D4;
static const uint8_t D5 = PIN_D5;
static const uint8_t D6 = PIN_D6;
static const uint8_t D7 = PIN_D7;
static const uint8_t D8 = PIN_D8;
static const uint8_t D9 = PIN_D9;
static const uint8_t D10 = PIN_D10;
static const uint8_t D11 = PIN_D11;
static const uint8_t D12 = PIN_D12;
static const uint8_t D13 = PIN_D13;
static const uint8_t D14 = PIN_D14;
static const uint8_t D15 = PIN_D15;
static const uint8_t D16 = PIN_D16;

static const uint8_t PA0 = PIN_D3;
static const uint8_t PA1 = PIN_D4;
static const uint8_t PA10 = PIN_D1;
static const uint8_t PA11 = PIN_D0;
static const uint8_t PA18 = PIN_D2;
static const uint8_t PA25 = PIN_D15;
static const uint8_t PB2 = PIN_D9;
static const uint8_t PB3 = PIN_D10;
static const uint8_t PB6 = PIN_D11;
static const uint8_t PB7 = PIN_D12;
static const uint8_t PB8 = PIN_D13;
static const uint8_t PB9 = PIN_D14;
static const uint8_t PB21 = PIN_D5;
static const uint8_t PB22 = PIN_D6;
static const uint8_t PB25 = PIN_D16;
static const uint8_t PB26 = PIN_D7;
static const uint8_t PB27 = PIN_D8;
#endif

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

static const uint8_t SS = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK = PIN_SPI_SCK;
