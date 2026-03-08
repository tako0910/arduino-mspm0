#include "Arduino.h"

int main(void)
{
    init();
    initVariant();
    setup();
    while (true) {
        loop();
        if (arduino::serialEventRun) {
            arduino::serialEventRun();
        }
        yield();
    }
}
