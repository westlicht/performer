#pragma once

template<typename T>
static T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}
