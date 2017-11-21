#include "DIO.h"

#include <libopencm3/stm32/rcc.h>

void DIO::init() {
    rcc_periph_clock_enable(RCC_GPIOB);

    clockInput.init();
    resetInput.init();
    clockOutput.init();
    resetOutput.init();
}
