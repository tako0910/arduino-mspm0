#pragma once

#include <Arduino.h>
#include <HardwareSPI.h>

namespace arduino {

class MspM0SPI : public SPIClass {
public:
    MspM0SPI();

    uint8_t transfer(uint8_t data) override;
    uint16_t transfer16(uint16_t data) override;
    void transfer(void *buf, size_t count) override;

    void usingInterrupt(int interruptNumber) override;
    void notUsingInterrupt(int interruptNumber) override;
    void beginTransaction(SPISettings settings) override;
    void endTransaction(void) override;

    void attachInterrupt() override;
    void detachInterrupt() override;

    void begin() override;
    void end() override;

private:
    bool _begun;
    SPISettings _settings;

    void applySettings(const SPISettings& settings);
};

} // namespace arduino

using arduino::SPIClass;
using arduino::SPISettings;
using arduino::SPI_MODE0;
using arduino::SPI_MODE1;
using arduino::SPI_MODE2;
using arduino::SPI_MODE3;

extern arduino::MspM0SPI SPI;
