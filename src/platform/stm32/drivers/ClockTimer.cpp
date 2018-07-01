#include "ClockTimer.h"

#include "SystemConfig.h"

#include "core/Debug.h"

#include "os/os.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include <functional>

#define TIMER TIM5

static ClockTimer::Listener *g_listener;

void ClockTimer::init() {
    rcc_periph_clock_enable(RCC_TIM5);
    nvic_set_priority(NVIC_TIM5_IRQ, CONFIG_CLOCKTIMER_IRQ_PRIORITY);
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
    _period = us;
    timer_set_period(TIMER, us - 1);
    timer_set_counter(TIMER, std::min(timer_get_counter(TIMER), us - 1));
}

void ClockTimer::setListener(Listener *listener) {
    os::InterruptLock lock;
    g_listener = listener;
}

void tim5_isr() {
    if (timer_get_flag(TIM5, TIM_SR_UIF)) {
        timer_clear_flag(TIM5, TIM_SR_UIF);
        if (g_listener) {
            g_listener->onClockTimerTick();
        }
    }
}
