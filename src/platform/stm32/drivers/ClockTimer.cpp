#include "ClockTimer.h"

#include "core/Debug.h"

#include <functional>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#define TIMER TIM5

static std::function<void()> g_handler;

void ClockTimer::init() {
    rcc_periph_clock_enable(RCC_TIM5);
    nvic_enable_irq(NVIC_TIM5_IRQ);
    rcc_periph_reset_pulse(RST_TIM5);

    timer_disable_preload(TIMER);
    timer_continuous_mode(TIMER);

    // set to 1mhz
    uint32_t prescaler = (rcc_apb1_frequency * 2) / 1000000 - 1;
    timer_set_prescaler(TIMER, prescaler);

    timer_enable_update_event(TIMER);
    timer_enable_irq(TIMER, TIM_DIER_UIE);
}

void ClockTimer::reset() {
}

void ClockTimer::enable() {
    timer_enable_irq(TIMER, TIM_DIER_UIE);
    timer_enable_counter(TIMER);
}

void ClockTimer::disable() {
    timer_disable_irq(TIMER, TIM_DIER_UIE);
    timer_disable_counter(TIMER);
}

void ClockTimer::setPeriod(uint32_t us) {
    timer_set_period(TIMER, us - 1);
}

void ClockTimer::setHandler(std::function<void()> handler) {
    timer_disable_irq(TIMER, TIM_DIER_UIE);
    g_handler = handler;
    timer_enable_irq(TIMER, TIM_DIER_UIE);
}

void tim5_isr() {    
    if (timer_get_flag(TIM5, TIM_SR_UIF)) {
        timer_clear_flag(TIM5, TIM_SR_UIF);
        g_handler();
    }
}
