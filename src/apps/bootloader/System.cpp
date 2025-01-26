#include "System.h"
#include "Config.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/cm3/systick.h>

uint32_t System::_ticks;

void System::init() {
    // Base board frequency is 168MHz
    rcc_clock_setup_pll(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    // clock frequencies:
    // ahb_frequency  = 168000000
    // apb1_frequency = 42000000
    // apb2_frequency = 84000000
}

void System::deinit() {
}

void System::reset() {
    iwdg_start();
}

void System::startSysTick() {
    // 1ms clock rate
    systick_set_reload(CONFIG_CPU_FREQUENCY / CONFIG_TICK_FREQUENCY);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_interrupt_enable();
    systick_counter_enable();
}

void System::stopSysTick() {
    systick_interrupt_disable();
    systick_counter_disable();
}
