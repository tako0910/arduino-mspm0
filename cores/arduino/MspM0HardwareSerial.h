#pragma once

#include <string.h>

#include "api/HardwareSerial.h"

#ifndef SERIAL_TX_BUFFER_SIZE
#define SERIAL_TX_BUFFER_SIZE 16
#endif

#ifndef SERIAL_RX_BUFFER_SIZE
#define SERIAL_RX_BUFFER_SIZE 16
#endif

namespace arduino {

class MspM0HardwareSerial : public HardwareSerial {
public:
    MspM0HardwareSerial();

    using Print::write;

    void begin(unsigned long baud) override;
    void begin(unsigned long baud, uint16_t config) override;
    void end() override;
    int available(void) override;
    int peek(void) override;
    int read(void) override;
    void flush(void) override;
    size_t write(uint8_t value) override;
    size_t write(int n) { return write(static_cast<uint8_t>(n)); }
    operator bool() override;

    void handleInterrupt();

private:
    uint8_t _rxBuffer[SERIAL_RX_BUFFER_SIZE];
    volatile uint8_t _head;
    volatile uint8_t _tail;
#if SERIAL_TX_BUFFER_SIZE > 0
    uint8_t _txBuffer[SERIAL_TX_BUFFER_SIZE];
    volatile uint8_t _txHead;
    volatile uint8_t _txTail;
#endif
    bool _begun;

    void pushByte(uint8_t value);
#if SERIAL_TX_BUFFER_SIZE > 0
    void flushTxFifo(void);
#endif
};

} // namespace arduino
