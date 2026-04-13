#include "MspM0HardwareSerial.h"

#include "Arduino.h"
#include "MspM0Core.h"

namespace arduino {

static MspM0HardwareSerial SerialImpl;

MspM0HardwareSerial::MspM0HardwareSerial() : _rxBuffer{0}, _head(0), _tail(0),
#if SERIAL_TX_BUFFER_SIZE > 0
    _txBuffer{0}, _txHead(0), _txTail(0),
#endif
    _begun(false)
{
}

void MspM0HardwareSerial::begin(unsigned long baud)
{
    begin(baud, SERIAL_8N1);
}

void MspM0HardwareSerial::begin(unsigned long baud, uint16_t config)
{
    _head = 0;
    _tail = 0;
#if SERIAL_TX_BUFFER_SIZE > 0
    _txHead = 0;
    _txTail = 0;
#endif
    mspm0_uart0_init(baud, config);
    DL_UART_enableFIFOs(MSPM0_SERIAL_INST);
    DL_UART_setRXFIFOThreshold(MSPM0_SERIAL_INST, DL_UART_RX_FIFO_LEVEL_ONE_ENTRY);
    DL_UART_setTXFIFOThreshold(MSPM0_SERIAL_INST, DL_UART_TX_FIFO_LEVEL_EMPTY);
    DL_UART_clearInterruptStatus(MSPM0_SERIAL_INST, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_TX);
    NVIC_ClearPendingIRQ(MSPM0_SERIAL_INT_IRQN);
    NVIC_EnableIRQ(MSPM0_SERIAL_INT_IRQN);
    _begun = true;
}

void MspM0HardwareSerial::end()
{
    DL_UART_disableInterrupt(MSPM0_SERIAL_INST, DL_UART_INTERRUPT_RX | DL_UART_INTERRUPT_TX);
    NVIC_DisableIRQ(MSPM0_SERIAL_INT_IRQN);
    DL_UART_Main_disable(MSPM0_SERIAL_INST);
    _head = 0;
    _tail = 0;
#if SERIAL_TX_BUFFER_SIZE > 0
    _txHead = 0;
    _txTail = 0;
#endif
    _begun = false;
}

int MspM0HardwareSerial::available(void)
{
    return (_head >= _tail) ? (_head - _tail) : (SERIAL_RX_BUFFER_SIZE - (_tail - _head));
}

int MspM0HardwareSerial::peek(void)
{
    if (_head == _tail) {
        return -1;
    }
    return _rxBuffer[_tail];
}

int MspM0HardwareSerial::read(void)
{
    uint8_t value;
    if (_head == _tail) {
        return -1;
    }
    value = _rxBuffer[_tail];
    _tail = (uint8_t) ((_tail + 1U) % SERIAL_RX_BUFFER_SIZE);
    return value;
}

void MspM0HardwareSerial::flush(void)
{
#if SERIAL_TX_BUFFER_SIZE > 0
    while (_txHead != _txTail) {
        yield();
    }
#endif
    while (DL_UART_Main_isBusy(MSPM0_SERIAL_INST)) {
    }
}

size_t MspM0HardwareSerial::write(uint8_t value)
{
    if (!_begun) {
        begin(115200UL, SERIAL_8N1);
    }
#if SERIAL_TX_BUFFER_SIZE > 0
    {
        uint8_t nextHead = (uint8_t) ((_txHead + 1U) % SERIAL_TX_BUFFER_SIZE);
        while (nextHead == _txTail) {
            yield();
        }
        _txBuffer[_txHead] = value;
        _txHead = nextHead;
        flushTxFifo();
        DL_UART_enableInterrupt(MSPM0_SERIAL_INST, DL_UART_INTERRUPT_TX);
    }
#else
    DL_UART_Main_transmitDataBlocking(MSPM0_SERIAL_INST, value);
#endif
    return 1;
}

MspM0HardwareSerial::operator bool()
{
    return true;
}

void MspM0HardwareSerial::pushByte(uint8_t value)
{
    uint8_t nextHead = (uint8_t) ((_head + 1U) % SERIAL_RX_BUFFER_SIZE);
    if (nextHead == _tail) {
        return;
    }
    _rxBuffer[_head] = value;
    _head = nextHead;
}

#if SERIAL_TX_BUFFER_SIZE > 0
void MspM0HardwareSerial::flushTxFifo(void)
{
    while ((_txHead != _txTail) && !DL_UART_isTXFIFOFull(MSPM0_SERIAL_INST)) {
        DL_UART_transmitData(MSPM0_SERIAL_INST, _txBuffer[_txTail]);
        _txTail = (uint8_t) ((_txTail + 1U) % SERIAL_TX_BUFFER_SIZE);
    }
    if (_txHead == _txTail) {
        DL_UART_disableInterrupt(MSPM0_SERIAL_INST, DL_UART_INTERRUPT_TX);
    }
}
#endif

void MspM0HardwareSerial::handleInterrupt()
{
    DL_UART_IIDX pending = DL_UART_getPendingInterrupt(MSPM0_SERIAL_INST);
    if (pending == DL_UART_IIDX_RX) {
        while (!DL_UART_isRXFIFOEmpty(MSPM0_SERIAL_INST)) {
            pushByte((uint8_t) DL_UART_receiveData(MSPM0_SERIAL_INST));
        }
        DL_UART_clearInterruptStatus(MSPM0_SERIAL_INST, DL_UART_INTERRUPT_RX);
    }
#if SERIAL_TX_BUFFER_SIZE > 0
    else if (pending == DL_UART_IIDX_TX) {
        flushTxFifo();
        DL_UART_clearInterruptStatus(MSPM0_SERIAL_INST, DL_UART_INTERRUPT_TX);
    }
#endif
}

extern "C" void MSPM0_SERIAL_IRQHandler(void)
{
    SerialImpl.handleInterrupt();
}

} // namespace arduino

arduino::HardwareSerial& Serial = arduino::SerialImpl;
