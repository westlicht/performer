#pragma once

#include <cstdint>

namespace sim {

static float adcToVoltage(uint16_t adc) {
    float normalized = (0xffff - adc) / float(0xffff);
    return (normalized - 0.5f) * 10.f;
}

static float dacToVoltage(uint16_t dac) {
    // In ideal DAC/OpAmp configuration we get:
    // 0     ->  5.17V
    // 32768 -> -5.25V
    // it follows:
    // 534   ->  5.00V
    // 31981 -> -5.00V
    static constexpr float value0 = 31981.f;
    static constexpr float value1 = 534.f;
    return (dac - value0) / (value1 - value0) * 10.f - 5.f;
}

} // namespace sim
