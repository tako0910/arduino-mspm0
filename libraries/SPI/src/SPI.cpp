#include "SPI.h"

#include "MspM0Core.h"

namespace {

static DL_SPI_FRAME_FORMAT spi_frame_format_for_mode(arduino::SPIMode mode)
{
    switch (mode) {
    case arduino::SPI_MODE1:
        return DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA1;
    case arduino::SPI_MODE2:
        return DL_SPI_FRAME_FORMAT_MOTO3_POL1_PHA0;
    case arduino::SPI_MODE3:
        return DL_SPI_FRAME_FORMAT_MOTO3_POL1_PHA1;
    case arduino::SPI_MODE0:
    default:
        return DL_SPI_FRAME_FORMAT_MOTO3_POL0_PHA0;
    }
}

static DL_SPI_CLOCK_DIVIDE_RATIO spi_clock_divider_for_frequency(uint32_t frequency)
{
    static const DL_SPI_CLOCK_DIVIDE_RATIO kDividers[] = {
        DL_SPI_CLOCK_DIVIDE_RATIO_1,
        DL_SPI_CLOCK_DIVIDE_RATIO_2,
        DL_SPI_CLOCK_DIVIDE_RATIO_3,
        DL_SPI_CLOCK_DIVIDE_RATIO_4,
        DL_SPI_CLOCK_DIVIDE_RATIO_5,
        DL_SPI_CLOCK_DIVIDE_RATIO_6,
        DL_SPI_CLOCK_DIVIDE_RATIO_7,
        DL_SPI_CLOCK_DIVIDE_RATIO_8,
    };
    uint32_t divider;

    if (frequency == 0UL) {
        return DL_SPI_CLOCK_DIVIDE_RATIO_8;
    }

    divider = (MSPM0_F_CPU + frequency - 1UL) / frequency;
    if (divider < 1UL) {
        divider = 1UL;
    } else if (divider > 8UL) {
        divider = 8UL;
    }

    return kDividers[divider - 1UL];
}

static void spi_drain_rx_fifo(void)
{
    while (!DL_SPI_isRXFIFOEmpty(SPI0)) {
        (void) DL_SPI_receiveData8(SPI0);
    }
}

} // namespace

namespace arduino {

MspM0SPI::MspM0SPI() : _begun(false), _settings(DEFAULT_SPI_SETTINGS)
{
}

void MspM0SPI::begin()
{
    mspm0_spi0_init();
    pinMode(PIN_SPI_SS, OUTPUT);
    digitalWrite(PIN_SPI_SS, HIGH);
    applySettings(_settings);
    _begun = true;
}

void MspM0SPI::end()
{
    DL_SPI_disable(SPI0);
    _begun = false;
}

uint8_t MspM0SPI::transfer(uint8_t data)
{
    if (!_begun) {
        begin();
    }

    spi_drain_rx_fifo();
    DL_SPI_transmitDataBlocking8(SPI0, data);
    return DL_SPI_receiveDataBlocking8(SPI0);
}

uint16_t MspM0SPI::transfer16(uint16_t data)
{
    DL_SPI_DATA_SIZE priorSize;
    uint16_t result;

    if (!_begun) {
        begin();
    }

    priorSize = DL_SPI_getDataSize(SPI0);
    DL_SPI_disable(SPI0);
    DL_SPI_setDataSize(SPI0, DL_SPI_DATA_SIZE_16);
    DL_SPI_enable(SPI0);
    spi_drain_rx_fifo();
    DL_SPI_transmitDataBlocking16(SPI0, data);
    result = DL_SPI_receiveDataBlocking16(SPI0);
    DL_SPI_disable(SPI0);
    DL_SPI_setDataSize(SPI0, priorSize);
    DL_SPI_enable(SPI0);
    return result;
}

void MspM0SPI::transfer(void *buf, size_t count)
{
    uint8_t *bytes = static_cast<uint8_t *>(buf);
    size_t index;

    for (index = 0; index < count; ++index) {
        bytes[index] = transfer(bytes[index]);
    }
}

void MspM0SPI::usingInterrupt(int interruptNumber)
{
    (void) interruptNumber;
}

void MspM0SPI::notUsingInterrupt(int interruptNumber)
{
    (void) interruptNumber;
}

void MspM0SPI::beginTransaction(SPISettings settings)
{
    if (!_begun) {
        begin();
    }
    applySettings(settings);
}

void MspM0SPI::endTransaction(void)
{
}

void MspM0SPI::attachInterrupt()
{
}

void MspM0SPI::detachInterrupt()
{
}

void MspM0SPI::applySettings(const SPISettings& settings)
{
    DL_SPI_ClockConfig clockConfig = {
        .clockSel = DL_SPI_CLOCK_BUSCLK,
        .divideRatio = spi_clock_divider_for_frequency(settings.getClockFreq()),
    };

    _settings = settings;

    DL_SPI_disable(SPI0);
    DL_SPI_setClockConfig(SPI0, &clockConfig);
    DL_SPI_setFrameFormat(SPI0, spi_frame_format_for_mode(settings.getDataMode()));
    DL_SPI_setBitOrder(SPI0, (settings.getBitOrder() == LSBFIRST) ?
        DL_SPI_BIT_ORDER_LSB_FIRST : DL_SPI_BIT_ORDER_MSB_FIRST);
    DL_SPI_setChipSelect(SPI0, DL_SPI_CHIP_SELECT_NONE);
    DL_SPI_setDataSize(SPI0, DL_SPI_DATA_SIZE_8);
    DL_SPI_enable(SPI0);
}

} // namespace arduino

arduino::MspM0SPI SPI;
