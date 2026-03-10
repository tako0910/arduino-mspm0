#pragma once

#include <string.h>

#include "api/ArduinoAPI.h"
#include "avr/dtostrf.h"
#include "api/HardwareSerial.h"
#include <variant.h>
#include <pins_arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

void interrupts(void);
void noInterrupts(void);
void analogReadResolution(int bits);
void analogWriteResolution(int bits);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
inline void pinMode(pin_size_t pinNumber, int mode)
{
    pinMode(pinNumber, static_cast<PinMode>(mode));
}

inline void digitalWrite(pin_size_t pinNumber, int status)
{
    digitalWrite(pinNumber, static_cast<PinStatus>(status));
}

namespace arduino {
class MspM0HardwareSerial;
}
using arduino::HardwareSerial;
#endif

extern HardwareSerial& Serial;

#define SERIAL_PORT_MONITOR Serial
#define SERIAL_PORT_HARDWARE Serial

#ifndef AR_DEFAULT
#define AR_DEFAULT 0
#endif

#ifndef AR_INTERNAL_1V4
#define AR_INTERNAL_1V4 1
#endif

#ifndef AR_INTERNAL_2V5
#define AR_INTERNAL_2V5 2
#endif

#ifndef F
#define F(str) (reinterpret_cast<const __FlashStringHelper *>(str))
#endif
