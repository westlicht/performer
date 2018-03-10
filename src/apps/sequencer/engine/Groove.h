#pragma once

#include <cstdint>

namespace Groove {

static uint32_t remap(uint32_t value, uint32_t range, uint32_t newRange) {
    return (value * newRange) / range;
}

static uint32_t swing(uint32_t tick, uint32_t base, int swing) {
    uint32_t period = 2 * base;

    uint32_t beat = tick / period;
    tick %= period;

    if (tick <= base) {
        tick = remap(tick, base, (period * swing) / 100);
    } else {
        tick = period - remap(period - tick, base, (period * (100 - swing)) / 100);
    }

    return beat * period + tick;
}

} // namespace Groove
