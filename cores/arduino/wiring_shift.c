#include "ArduinoCompat.h"

void shiftOut(pin_size_t dataPin, pin_size_t clockPin, BitOrder bitOrder, uint8_t value)
{
    uint8_t i;
    for (i = 0; i < 8U; i++) {
        uint8_t bitValue = (bitOrder == LSBFIRST) ? ((value >> i) & 0x01U) : ((value >> (7U - i)) & 0x01U);
        digitalWrite(dataPin, bitValue ? HIGH : LOW);
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}

uint8_t shiftIn(pin_size_t dataPin, pin_size_t clockPin, BitOrder bitOrder)
{
    uint8_t value = 0;
    uint8_t i;
    for (i = 0; i < 8U; i++) {
        digitalWrite(clockPin, HIGH);
        if (bitOrder == LSBFIRST) {
            value |= ((digitalRead(dataPin) == HIGH) ? 1U : 0U) << i;
        } else {
            value |= ((digitalRead(dataPin) == HIGH) ? 1U : 0U) << (7U - i);
        }
        digitalWrite(clockPin, LOW);
    }
    return value;
}
