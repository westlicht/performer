#include "MIDI.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#define MIDI_USART USART2
#define MIDI_RCC RCC_USART2
#define MIDI_NVIC_IRQ NVIC_USART2_IRQ

static MIDI *g_midi = nullptr;

void MIDI::init() {
    g_midi = this;

#if 0
    // setup GPIO pins
    rcc_periph_clock_enable(RCC_GPIOD);
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);
    gpio_set_af(GPIOD, GPIO_AF7, GPIO8 | GPIO9);

    // setup usart
    rcc_periph_clock_enable(MIDI_RCC);
    usart_set_baudrate(MIDI_USART, 31250);
    usart_set_databits(MIDI_USART, 8);
    usart_set_stopbits(MIDI_USART, USART_STOPBITS_1);
    usart_set_mode(MIDI_USART, USART_MODE_TX_RX);
    usart_set_parity(MIDI_USART, USART_PARITY_NONE);
    usart_set_flow_control(MIDI_USART, USART_FLOWCONTROL_NONE);
    usart_enable(MIDI_USART);

    // nvic_set_priority(CONSOLE_NVIC_IRQ, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    nvic_enable_irq(MIDI_NVIC_IRQ);
#endif
}

void MIDI::send(const MIDIMessage &message) {
    for (uint8_t i = 0; i < message.length(); ++i) {
        send(message.raw()[i]);
    }
}

bool MIDI::recv(MIDIMessage *message) {
    while (!_rxBuffer.empty()) {
        if (_midiParser.feed(_rxBuffer.read())) {
            *message = _midiParser.message();
            return true;
        }
    }
    return false;
}

void MIDI::setRecvFilter(std::function<bool(uint8_t)> filter) {
    _filter = filter;
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
        if (!_filter || !_filter(data)) {
            _rxBuffer.write(data);
        }
    }
}

void usart2_isr() {
    g_midi->handleIrq();
}
