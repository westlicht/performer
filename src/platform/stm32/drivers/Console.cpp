#include "Console.h"

#include "SystemConfig.h"

#include "os/os.h"
#include "core/utils/RingBuffer.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#define CONSOLE_USART USART1

void Console::init() {
    // setup GPIO pins
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9 | GPIO10);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO9 | GPIO10);

    // setup usart
    rcc_periph_clock_enable(RCC_USART1);
    usart_set_baudrate(CONSOLE_USART, 115200);
    usart_set_databits(CONSOLE_USART, 8);
    usart_set_stopbits(CONSOLE_USART, USART_STOPBITS_1);
    usart_set_mode(CONSOLE_USART, USART_MODE_TX_RX);
    usart_set_parity(CONSOLE_USART, USART_PARITY_NONE);
    usart_set_flow_control(CONSOLE_USART, USART_FLOWCONTROL_NONE);
    usart_enable(CONSOLE_USART);

    nvic_set_priority(NVIC_USART1_IRQ, CONFIG_CONSOLE_IRQ_PRIORITY);
    nvic_enable_irq(NVIC_USART1_IRQ);
}

void Console::write(char c) {
    if (c == '\n') {
        send('\r');
    }
    send(c);
}

void Console::write(const char *s) {
    while (*s != '\0') {
        write(*s++);
    }
}

void Console::write(const char *s, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        write(s[i]);
    }
}

void Console::write(const std::string &s) {
    for (const auto c : s) {
        write(char(c));
    }
}

static RingBuffer<char, 256> txBuffer;
static volatile uint32_t txActive;

void Console::send(char c) {
    while (txBuffer.writable() == 0) {}

    txBuffer.write(c);

    if (!txActive) {
        txActive = 1;
        usart_wait_send_ready(CONSOLE_USART);
        usart_send(CONSOLE_USART, txBuffer.read());
        usart_enable_tx_interrupt(CONSOLE_USART);
    }
}

void usart1_isr() {
    if (usart_get_flag(CONSOLE_USART, USART_SR_TXE)) {
        usart_disable_tx_interrupt(CONSOLE_USART);
        if (txBuffer.readable() > 0) {
            usart_send(CONSOLE_USART, txBuffer.read());
            usart_enable_tx_interrupt(CONSOLE_USART);
        } else {
            txActive = 0;
        }
    }
}

extern "C" {

int _write(int file, char *data, int len) {
    Console::write(data, len);
    return len;
}

}
