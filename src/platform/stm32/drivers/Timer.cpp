#include "Timer.h"

#include <functional>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

static struct TimerInfo {
    uint32_t timer;
    enum rcc_periph_clken rcc;
    enum rcc_periph_rst rst;
    uint8_t irq;
} g_timerInfos[Timer::HardwareTimerCount] = {
    // { TIM1, RCC_TIM1, RST_TIM1, NVIC_TIM1_IRQ },
    // { TIM2, RCC_TIM2, RST_TIM2, NVIC_TIM2_IRQ },
    { TIM3, RCC_TIM3, RST_TIM3, NVIC_TIM3_IRQ },
    { TIM4, RCC_TIM4, RST_TIM4, NVIC_TIM4_IRQ },
    { TIM5, RCC_TIM5, RST_TIM5, NVIC_TIM5_IRQ },
    // { TIM6, RCC_TIM6, RST_TIM6, NVIC_TIM6_IRQ },
    // { TIM7, RCC_TIM7, RST_TIM7, NVIC_TIM7_IRQ },
};

static std::function<void()> g_handlers[Timer::HardwareTimerCount];

Timer::Timer(HardwareTimer hardwareTimer) :
    _hardwareTimer(hardwareTimer)
{
}

void Timer::init() {
    const auto &info = g_timerInfos[_hardwareTimer];
    rcc_periph_clock_enable(info.rcc);
    nvic_enable_irq(info.irq);
    rcc_periph_reset_pulse(info.rst);

    timer_disable_preload(info.timer);
    timer_continuous_mode(info.timer);

    // timer_enable_update_event(TIMER);

}

void Timer::reset() {
    const auto &info = g_timerInfos[_hardwareTimer];
    timer_reset(info.timer);
}

void Timer::enable() {
    const auto &info = g_timerInfos[_hardwareTimer];
    timer_enable_irq(info.timer, TIM_DIER_UIE);
    timer_enable_counter(info.timer);
}

void Timer::disable() {
    const auto &info = g_timerInfos[_hardwareTimer];
    timer_disable_irq(info.timer, TIM_DIER_UIE);
    timer_disable_counter(info.timer);
}

void Timer::setPeriod(uint32_t us) {

}

void Timer::setHandler(std::function<void()> handler) {
    const auto &info = g_timerInfos[_hardwareTimer];
    timer_disable_irq(info.timer, TIM_DIER_UIE);
    g_handlers[_hardwareTimer] = handler;
    timer_enable_irq(info.timer, TIM_DIER_UIE);
}

void tim3_isr() {
    if (timer_get_flag(TIM3, TIM_SR_UIF)) {
        timer_clear_flag(TIM3, TIM_SR_UIF);
        g_handlers[Timer::HardwareTimer3]();
    }
}

#if 0
{
    rcc_periph_clock_enable(RCC_TIM2);
    nvic_enable_irq(NVIC_TIM2_IRQ);
    rcc_periph_reset_pulse(RST_TIM2);

    timer_disable_preload(TIMER);
    timer_continuous_mode(TIMER);

    // set to 10mhz
    timer_set_prescaler(TIMER, (rcc_apb1_frequency * 2) / 10000000 - 1);
    // count microseconds
    timer_set_period(TIMER, 10);

    timer_enable_update_event(TIMER);
    timer_enable_counter(TIMER);
    timer_enable_irq(TIMER, TIM_DIER_UIE);
}

void tim2_isr() {
    if (timer_get_flag(TIMER, TIM_SR_UIF)) {
        timer_clear_flag(TIMER, TIM_SR_UIF);
        HighResolutionTimer::tick();
    }
}
#endif
