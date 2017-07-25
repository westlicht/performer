#include "MIDI.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#define MIDI_USART USART2

static MIDI *g_midi = nullptr;

void MIDI::init() {
    g_midi = this;
}

void MIDI::setRecvFilter(std::function<bool(uint8_t)> filter) {
    _recvFilter = filter;
}

void MIDI::send(uint8_t data) {
    while (_txBuffer.writable() == 0) {}

    _txBuffer.write(data);

    if (!_txActive) {
        _txActive = true;
        usart_wait_send_ready(MIDI_USART);
        usart_send(MIDI_USART, _txBuffer.read());
        usart_enable_tx_interrupt(MIDI_USART);
    }
}

void MIDI::handleIrq() {
    if (usart_get_flag(MIDI_USART, USART_SR_TXE)) {
        usart_disable_tx_interrupt(MIDI_USART);
        if (_txBuffer.readable() > 0) {
            usart_send(MIDI_USART, _txBuffer.read());
            usart_enable_tx_interrupt(MIDI_USART);
        } else {
            _txActive = 0;
        }
    }
    if (usart_get_flag(MIDI_USART, USART_SR_RXNE)) {
        if (_rxBuffer.writable() == 0) {
            // overflow
        }
        uint8_t data = usart_recv(MIDI_USART);
        if (!_recvFilter || !_recvFilter(data)) {
            _rxBuffer.write(data);
        }
    }
}


void usart2_isr() {
    g_midi->handleIrq();
}
