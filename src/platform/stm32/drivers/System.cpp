#include "System.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

volatile uint32_t System::_ticks;


// /* Called when systick fires */
// void sys_tick_handler(void) {
//     System::tick();
// }

void System::init() {
    /* Base board frequency, set to 168Mhz */
    rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    /* 1ms clock rate */
    systick_set_reload(168000000 / 1000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();

    /* this done last */
    systick_interrupt_enable();
}

void System::tick() {
    ++_ticks;
}