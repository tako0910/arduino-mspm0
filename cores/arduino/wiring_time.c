#include "ArduinoCompat.h"
#include "MspM0Core.h"

volatile uint32_t g_millis_count;

static void delay_ms_busy(unsigned long ms)
{
    while (ms-- > 0UL) {
        delay_cycles(F_CPU / 1000UL);
    }
}

void SysTick_Handler(void)
{
    g_millis_count++;
}

unsigned long millis(void)
{
    return g_millis_count;
}

unsigned long micros(void)
{
    uint32_t ms1;
    uint32_t ms2;
    uint32_t ticks;
    uint32_t period = DL_SYSTICK_getPeriod();

    do {
        ms1 = g_millis_count;
        ticks = DL_SYSTICK_getValue();
        ms2 = g_millis_count;
    } while (ms1 != ms2);

    return (ms1 * 1000UL) + (((period - ticks) * 1000UL) / period);
}

void delay(unsigned long ms)
{
    uint32_t start;
    uint32_t last;
    uint32_t stalledLoops = 0U;

    if (ms == 0UL) {
        return;
    }

    if ((__get_PRIMASK() & 1U) != 0U) {
        delay_ms_busy(ms);
        return;
    }

    start = millis();
    last = start;
    while ((millis() - start) < ms) {
        uint32_t current = millis();

        if (current == last) {
            stalledLoops++;
            if (stalledLoops > 1000U) {
                delay_ms_busy(ms - (current - start));
                return;
            }
        } else {
            last = current;
            stalledLoops = 0U;
        }
        yield();
    }
}

void delayMicroseconds(unsigned int us)
{
    if (us == 0U) {
        return;
    }
    delay_cycles((uint32_t) us * (F_CPU / 1000000UL));
}

unsigned long pulseIn(pin_size_t pin, uint8_t state, unsigned long timeout)
{
    return pulseInLong(pin, state, timeout);
}

unsigned long pulseInLong(pin_size_t pin, uint8_t state, unsigned long timeout)
{
    uint32_t start = micros();
    uint32_t timeoutAt = start + timeout;
    PinStatus target = state ? HIGH : LOW;
    uint32_t pulseStart;

    while (digitalRead(pin) == target) {
        if ((micros() - start) >= timeout) {
            return 0;
        }
    }

    while (digitalRead(pin) != target) {
        if ((micros() - start) >= timeout) {
            return 0;
        }
    }

    pulseStart = micros();
    while ((digitalRead(pin) == target) && ((micros() - pulseStart) < timeout)) {
    }

    (void) timeoutAt;
    return micros() - pulseStart;
}
