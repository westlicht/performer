#include "Console.h"

#include "os/os.h"
#include "core/utils/RingBuffer.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#define CONSOLE_USART USART3
#define CONSOLE_RCC RCC_USART3
#define CONSOLE_NVIC_IRQ NVIC_USART3_IRQ


void Console::init() {
    // setup GPIO pins
    rcc_periph_clock_enable(RCC_GPIOD);
    gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO8 | GPIO9);
    gpio_set_af(GPIOD, GPIO_AF7, GPIO8 | GPIO9);

    // setup usart
    rcc_periph_clock_enable(CONSOLE_RCC);
    usart_set_baudrate(CONSOLE_USART, 9600);
    usart_set_databits(CONSOLE_USART, 8);
    usart_set_stopbits(CONSOLE_USART, USART_STOPBITS_1);
    usart_set_mode(CONSOLE_USART, USART_MODE_TX_RX);
    usart_set_parity(CONSOLE_USART, USART_PARITY_NONE);
    usart_set_flow_control(CONSOLE_USART, USART_FLOWCONTROL_NONE);
    usart_enable(CONSOLE_USART);

    // nvic_set_priority(CONSOLE_NVIC_IRQ, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    nvic_enable_irq(CONSOLE_NVIC_IRQ);
}

void Console::write(char c) {
    send(c);
}

void Console::write(const char *s) {
    while (*s != '\0') {
        send(*s++);
    }
}

void Console::write(const char *s, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        send(s[i]);
    }
}

void Console::write(const std::string &s) {
    for (const auto c : s) {
        send(char(c));
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

void usart3_isr() {
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
