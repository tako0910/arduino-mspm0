#include "Wire.h"

#include "MspM0Core.h"

namespace {

static constexpr uint32_t kI2CTimeoutCycles = 240000UL;

static bool i2c_wait_until_clear(uint32_t mask)
{
    uint32_t timeout = kI2CTimeoutCycles;

    while ((DL_I2C_getControllerStatus(MSPM0_WIRE_INST) & mask) != 0U) {
        if (--timeout == 0U) {
            return false;
        }
    }

    return true;
}

static bool i2c_wait_until_set(uint32_t mask)
{
    uint32_t timeout = kI2CTimeoutCycles;

    while ((DL_I2C_getControllerStatus(MSPM0_WIRE_INST) & mask) == 0U) {
        if (--timeout == 0U) {
            return false;
        }
    }

    return true;
}

} // namespace

namespace arduino {

MspM0TwoWire::MspM0TwoWire() :
    _begun(false),
    _busActive(false),
    _txAddress(0U),
    _txBuffer{0},
    _rxBuffer{0},
    _txLength(0U),
    _rxLength(0U),
    _rxIndex(0U),
    _clock(100000UL),
    _onReceive(nullptr),
    _onRequest(nullptr)
{
}

void MspM0TwoWire::begin()
{
    mspm0_i2c0_init(_clock);
    _begun = true;
    _busActive = false;
    _txLength = 0U;
    _rxLength = 0U;
    _rxIndex = 0U;
}

void MspM0TwoWire::begin(uint8_t address)
{
    (void) address;
    begin();
}

void MspM0TwoWire::end()
{
    DL_I2C_disableController(MSPM0_WIRE_INST);
    _begun = false;
    _busActive = false;
}

void MspM0TwoWire::setClock(uint32_t freq)
{
    _clock = freq;
    if (_begun) {
        begin();
    }
}

void MspM0TwoWire::beginTransmission(uint8_t address)
{
    if (!_begun) {
        begin();
    }

    _txAddress = address;
    _txLength = 0U;
}

uint8_t MspM0TwoWire::endTransmission(bool stopBit)
{
    uint8_t sent = 0U;
    uint32_t status;

    if (!_begun) {
        begin();
    }

    if (!waitControllerReady()) {
        return 5U;
    }
    if (!_busActive && !waitControllerIdle()) {
        return 5U;
    }

    DL_I2C_flushControllerTXFIFO(MSPM0_WIRE_INST);
    DL_I2C_startControllerTransferAdvanced(MSPM0_WIRE_INST, _txAddress,
        DL_I2C_CONTROLLER_DIRECTION_TX, _txLength,
        DL_I2C_CONTROLLER_START_ENABLE,
        stopBit ? DL_I2C_CONTROLLER_STOP_ENABLE : DL_I2C_CONTROLLER_STOP_DISABLE,
        DL_I2C_CONTROLLER_ACK_ENABLE);

    while (sent < _txLength) {
        status = DL_I2C_getControllerStatus(MSPM0_WIRE_INST);
        if ((status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
            _busActive = false;
            return transferError();
        }
        if (!DL_I2C_isControllerTXFIFOFull(MSPM0_WIRE_INST)) {
            DL_I2C_transmitControllerData(MSPM0_WIRE_INST, _txBuffer[sent++]);
        }
    }

    if (!waitControllerReady()) {
        _busActive = false;
        return 5U;
    }

    status = DL_I2C_getControllerStatus(MSPM0_WIRE_INST);
    _busActive = !stopBit;
    if ((status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
        _busActive = false;
        return transferError();
    }
    if (stopBit && !waitControllerIdle()) {
        _busActive = false;
        return 5U;
    }

    return 0U;
}

uint8_t MspM0TwoWire::endTransmission(void)
{
    return endTransmission(true);
}

size_t MspM0TwoWire::requestFrom(uint8_t address, size_t len, bool stopBit)
{
    size_t index;
    uint32_t status;

    if (!_begun) {
        begin();
    }

    if (len > kBufferSize) {
        len = kBufferSize;
    }

    _rxLength = 0U;
    _rxIndex = 0U;

    if (!waitControllerReady()) {
        _busActive = false;
        return 0U;
    }
    if (!_busActive && !waitControllerIdle()) {
        return 0U;
    }

    DL_I2C_flushControllerRXFIFO(MSPM0_WIRE_INST);
    DL_I2C_startControllerTransferAdvanced(MSPM0_WIRE_INST, address,
        DL_I2C_CONTROLLER_DIRECTION_RX, (uint16_t) len,
        DL_I2C_CONTROLLER_START_ENABLE,
        stopBit ? DL_I2C_CONTROLLER_STOP_ENABLE : DL_I2C_CONTROLLER_STOP_DISABLE,
        DL_I2C_CONTROLLER_ACK_ENABLE);

    for (index = 0; index < len; ++index) {
        uint32_t timeout = kI2CTimeoutCycles;
        while (DL_I2C_isControllerRXFIFOEmpty(MSPM0_WIRE_INST)) {
            status = DL_I2C_getControllerStatus(MSPM0_WIRE_INST);
            if ((status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
                _busActive = false;
                return _rxLength;
            }
            if (--timeout == 0U) {
                _busActive = false;
                return _rxLength;
            }
        }
        _rxBuffer[_rxLength++] = DL_I2C_receiveControllerData(MSPM0_WIRE_INST);
    }

    if (!waitControllerReady()) {
        _busActive = false;
        return _rxLength;
    }

    status = DL_I2C_getControllerStatus(MSPM0_WIRE_INST);
    _busActive = !stopBit;
    if ((status & DL_I2C_CONTROLLER_STATUS_ERROR) != 0U) {
        _busActive = false;
    }
    if (_onReceive != nullptr) {
        _onReceive(_rxLength);
    }

    return _rxLength;
}

size_t MspM0TwoWire::requestFrom(uint8_t address, size_t len)
{
    return requestFrom(address, len, true);
}

void MspM0TwoWire::onReceive(void(*callback)(int))
{
    _onReceive = callback;
}

void MspM0TwoWire::onRequest(void(*callback)(void))
{
    _onRequest = callback;
}

int MspM0TwoWire::available()
{
    return (int) (_rxLength - _rxIndex);
}

int MspM0TwoWire::read()
{
    if (_rxIndex >= _rxLength) {
        return -1;
    }
    return _rxBuffer[_rxIndex++];
}

int MspM0TwoWire::peek()
{
    if (_rxIndex >= _rxLength) {
        return -1;
    }
    return _rxBuffer[_rxIndex];
}

void MspM0TwoWire::flush()
{
}

size_t MspM0TwoWire::write(uint8_t data)
{
    if (_txLength >= kBufferSize) {
        return 0U;
    }
    _txBuffer[_txLength++] = data;
    return 1U;
}

int MspM0TwoWire::availableForWrite()
{
    return (int) (kBufferSize - _txLength);
}

bool MspM0TwoWire::waitControllerReady() const
{
    return i2c_wait_until_clear(DL_I2C_CONTROLLER_STATUS_BUSY);
}

bool MspM0TwoWire::waitControllerIdle() const
{
    return i2c_wait_until_set(DL_I2C_CONTROLLER_STATUS_IDLE);
}

uint8_t MspM0TwoWire::transferError() const
{
    uint32_t status = DL_I2C_getControllerStatus(MSPM0_WIRE_INST);

    if ((status & DL_I2C_CONTROLLER_STATUS_ARBITRATION_LOST) != 0U) {
        return 4U;
    }
    if ((status & (DL_I2C_CONTROLLER_STATUS_ADDR_ACK | DL_I2C_CONTROLLER_STATUS_DATA_ACK)) != 0U) {
        return 2U;
    }
    return 4U;
}

} // namespace arduino

arduino::MspM0TwoWire Wire;
