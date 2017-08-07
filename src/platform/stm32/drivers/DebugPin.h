#pragma once

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define DEBUG_PORT GPIOC
#define DEBUG_PIN GPIO10

class DebugPin {
public:
    static void init() {
    	rcc_periph_clock_enable(RCC_GPIOC);
        gpio_mode_setup(DEBUG_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DEBUG_PIN);
    }

    static void low() {
        gpio_clear(DEBUG_PORT, DEBUG_PIN);
    }

    static void high() {
        gpio_set(DEBUG_PORT, DEBUG_PIN);
    }
};
