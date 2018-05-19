#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

#include "Config.h"

#include <cstdint>

namespace Delay {

template<uint64_t cycles>
static void nop() {
    asm volatile ("nop");
    nop<cycles - 1>();
}
template<>
void nop<0>() {}

static constexpr uint64_t ceil(double num) {
    return (static_cast<double>(static_cast<uint64_t>(num)) == num)
        ? static_cast<uint64_t>(num)
        : static_cast<uint64_t>(num) + ((num > 0) ? 1 : 0);
}

static constexpr uint64_t ns_to_cycles(uint64_t ns) {
    return ceil(double(ns) / (double(1000000000) / double(CONFIG_CPU_FREQUENCY)));
}

// nanosecond delay based on nops
template<uint64_t ns>
static void delay_ns() {
    nop<ns_to_cycles(ns)>();
}

// microsecond delay
static void delay_us(uint32_t us) {
    for (volatile uint32_t i = 0; i < us * 10; ++i) {
        delay_ns<100>();
    }
}

} // namespace Delay

#pragma GCC diagnostic pop
