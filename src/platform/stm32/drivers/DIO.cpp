#include "DIO.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/exti.h>

static DIO *g_dio;

DIO::DIO() {
    g_dio = this;
}

void DIO::init() {
    rcc_periph_clock_enable(RCC_GPIOB);

    clockInput.init();
    resetInput.init();
    clockOutput.init();
    resetOutput.init();

    rcc_periph_clock_enable(RCC_SYSCFG);

    nvic_enable_irq(NVIC_EXTI15_10_IRQ);

    exti_select_source(EXTI10 | EXTI11, GPIOB);
    exti_set_trigger(EXTI10 | EXTI11, EXTI_TRIGGER_BOTH);
    exti_enable_request(EXTI10 | EXTI11);
}

void exti15_10_isr(void) {
    if (exti_get_flag_status(EXTI10)) {
        exti_reset_request(EXTI10);
        g_dio->resetInput.interrupt();
    }
    if (exti_get_flag_status(EXTI11)) {
        exti_reset_request(EXTI11);
        g_dio->clockInput.interrupt();
    }
}
