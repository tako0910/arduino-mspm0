#pragma once

#include <Arduino.h>
#include "api/HardwareI2C.h"

namespace arduino {

class MspM0TwoWire : public HardwareI2C {
public:
    MspM0TwoWire();

    using Print::write;

    void begin() override;
    void begin(uint8_t address) override;
    void end() override;

    void setClock(uint32_t freq) override;

    void beginTransmission(uint8_t address) override;
    uint8_t endTransmission(bool stopBit) override;
    uint8_t endTransmission(void) override;

    size_t requestFrom(uint8_t address, size_t len, bool stopBit) override;
    size_t requestFrom(uint8_t address, size_t len) override;

    void onReceive(void(*callback)(int)) override;
    void onRequest(void(*callback)(void)) override;

    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    size_t write(uint8_t data) override;
    size_t write(int n) { return write(static_cast<uint8_t>(n)); }
    int availableForWrite() override;

private:
    static constexpr uint8_t kBufferSize = 32;

    bool _begun;
    bool _busActive;
    uint8_t _txAddress;
    uint8_t _txBuffer[kBufferSize];
    uint8_t _rxBuffer[kBufferSize];
    uint8_t _txLength;
    uint8_t _rxLength;
    uint8_t _rxIndex;
    uint32_t _clock;
    void (*_onReceive)(int);
    void (*_onRequest)(void);

    bool waitControllerReady() const;
    bool waitControllerIdle() const;
    uint8_t transferError() const;
};

} // namespace arduino

using arduino::MspM0TwoWire;
using TwoWire = arduino::MspM0TwoWire;

extern arduino::MspM0TwoWire Wire;
