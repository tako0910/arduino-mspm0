#include "ArduinoCompat.h"
#include "MspM0Core.h"

uint32_t SystemCoreClock = F_CPU;

extern void _exit(int status);

int atexit(void (*func)(void))
{
    (void) func;
    return 0;
}

void exit(int status)
{
    _exit(status);
}

void yield(void)
{
}

void init(void)
{
    mspm0_core_init();
    __enable_irq();
}

void interrupts(void)
{
    __enable_irq();
}

void noInterrupts(void)
{
    __disable_irq();
}
