#include "Arduino.h"

static unsigned long s_random_seed = 1;

long random(long max)
{
    return random(0, max);
}

long random(long minValue, long maxValue)
{
    long range;
    if (maxValue <= minValue) {
        return minValue;
    }
    s_random_seed = (1103515245UL * s_random_seed) + 12345UL;
    range = maxValue - minValue;
    return minValue + (long) ((s_random_seed >> 1) % (unsigned long) range);
}

void randomSeed(unsigned long seed)
{
    if (seed != 0UL) {
        s_random_seed = seed;
    }
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration)
{
    unsigned long startUs;
    unsigned long durationUs;
    unsigned long periodUs;
    unsigned long halfPeriodUs;

    if ((pin >= PINS_COUNT) || (frequency == 0U) || (duration == 0UL)) {
        return;
    }

    periodUs = 1000000UL / (unsigned long) frequency;
    if (periodUs < 2UL) {
        periodUs = 2UL;
    }
    halfPeriodUs = periodUs / 2UL;
    if (halfPeriodUs == 0UL) {
        halfPeriodUs = 1UL;
    }

    pinMode(pin, OUTPUT);
    startUs = micros();
    durationUs = duration * 1000UL;
    while ((unsigned long) (micros() - startUs) < durationUs) {
        digitalWrite(pin, HIGH);
        delayMicroseconds((unsigned int) halfPeriodUs);
        digitalWrite(pin, LOW);
        delayMicroseconds((unsigned int) halfPeriodUs);
    }
}

void noTone(uint8_t pin)
{
    digitalWrite(pin, LOW);
}
