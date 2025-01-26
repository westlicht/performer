#include "System.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/cm3/systick.h>

volatile uint32_t System::_ticks;

void System::init() {
    // Base board frequency is 168MHz
    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    // clock frequencies:
    // ahb_frequency  = 168000000
    // apb1_frequency = 42000000
    // apb2_frequency = 84000000

    // 1ms clock rate
    systick_set_reload(168000000 / 1000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
}

void System::reset() {
    iwdg_set_period_ms(0);
    iwdg_start();
}

void System::tick() {
    ++_ticks;
}

void System::startWatchdog(uint32_t ms) {
    iwdg_set_period_ms(ms);
    iwdg_start();
}

void System::resetWatchdog() {
    iwdg_reset();
}
