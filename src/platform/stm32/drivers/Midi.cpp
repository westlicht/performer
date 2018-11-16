#include "Midi.h"

#include "SystemConfig.h"

#include "os/os.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#define MIDI_USART USART6

static Midi *g_midi = nullptr;

void Midi::init() {
    g_midi = this;

    // setup GPIO pins
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6 | GPIO7);
    gpio_set_af(GPIOC, GPIO_AF8, GPIO6 | GPIO7);

    // setup usart
    rcc_periph_clock_enable(RCC_USART6);
    usart_set_baudrate(MIDI_USART, 31250);
    usart_set_databits(MIDI_USART, 8);
    usart_set_stopbits(MIDI_USART, USART_STOPBITS_1);
    usart_set_mode(MIDI_USART, USART_MODE_TX_RX);
    usart_set_parity(MIDI_USART, USART_PARITY_NONE);
    usart_set_flow_control(MIDI_USART, USART_FLOWCONTROL_NONE);
    usart_enable(MIDI_USART);

    usart_enable_rx_interrupt(MIDI_USART);

    nvic_set_priority(NVIC_USART6_IRQ, CONFIG_MIDI_IRQ_PRIORITY);
    nvic_enable_irq(NVIC_USART6_IRQ);
}

bool Midi::send(const MidiMessage &message) {
    for (uint8_t i = 0; i < message.length(); ++i) {
        send(message.raw()[i]);
    }

    return true;
}

bool Midi::recv(MidiMessage *message) {
    while (!_rxBuffer.empty()) {
        if (_midiParser.feed(_rxBuffer.read())) {
            *message = _midiParser.message();
            return true;
        }
    }
    return false;
}

void Midi::setRecvFilter(RecvFilter filter) {
    _recvFilter = filter;
}

void Midi::send(uint8_t data) {
    os::InterruptLock lock;

    // block until there is space in the tx buffer
    while (_txBuffer.full()) {
        usart_wait_send_ready(MIDI_USART);
        usart_send(MIDI_USART, _txBuffer.read());
    }

    _txBuffer.write(data);

    // start transmission if necessary
    if (!_txActive) {
        _txActive = 1;
        usart_wait_send_ready(MIDI_USART);
        usart_send(MIDI_USART, _txBuffer.read());
        usart_enable_tx_interrupt(MIDI_USART);
    }
}

void Midi::handleIrq() {
    os::InterruptLock lock;
    if (usart_get_flag(MIDI_USART, USART_SR_TXE)) {
        if (_txBuffer.empty()) {
            usart_disable_tx_interrupt(MIDI_USART);
            _txActive = 0;
        } else {
            usart_send(MIDI_USART, _txBuffer.read());
        }
    }
    if (usart_get_flag(MIDI_USART, USART_SR_RXNE)) {
        uint8_t data = usart_recv(MIDI_USART);
        if (!_recvFilter || !_recvFilter(data)) {
            if (_rxBuffer.full()) {
                // overflow
                ++_rxOverflow;
            }
            _rxBuffer.write(data);
        }
    }
}

void usart6_isr() {
    g_midi->handleIrq();
}
