#pragma once

#include <algorithm>

#include <cstdint>

template<typename T>
static T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

uint32_t nextPowerOfTwo(uint32_t value, bool strict = true);
uint32_t prevPowerOfTwo(uint32_t value, bool strict = true);
