#pragma once

#include <libopencm3/stm32/gpio.h>

#include <cstdint>

class DebugLed {
    uint32_t _port;
    uint16_t _pin;

public:
    DebugLed(uint32_t port, uint16_t pin) :
        _port(port),
        _pin(pin)
    {
        gpio_mode_setup(_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, _pin);
    }

    void set(bool value) {
        if (value) {
            on();
        } else {
            off();
        }
    }

    void on() {
        gpio_set(_port, _pin);
    }

    void off() {
        gpio_clear(_port, _pin);
    }

};
