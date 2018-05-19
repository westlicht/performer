#include "Console.h"

#include "Config.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include <cstdarg>

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
}

void Console::deinit() {
    rcc_periph_clock_disable(RCC_GPIOA);
    rcc_periph_clock_disable(RCC_USART1);
    rcc_periph_reset_pulse(RST_GPIOA);
    rcc_periph_reset_pulse(RST_USART1);
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

void Console::send(char c) {
    usart_wait_send_ready(CONSOLE_USART);
    usart_send(CONSOLE_USART, c);
}

extern "C" {

static char *_write(char *buf, void *user, int len) {
    Console::write(buf, len);
    return buf;
}

void printf(char const *fmt, ...) {
    va_list va;
    char buf[CONFIG_PRINTF_BUFFER];
    va_start(va, fmt);
    stbsp_vsprintfcb(&_write, buf, buf, fmt, va);
    va_end(va);
}

} // extern "C"
