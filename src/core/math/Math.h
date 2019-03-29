#pragma once

#include <algorithm>

#include <cmath>
#include <cstdint>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

template<typename T>
static T clamp(T value, T min, T max) {
    return std::max(min, std::min(max, value));
}

template<typename T, typename S>
static S lerp(T t, S a, S b) {
    return (T(1) - t) * a + t * b;
}

uint32_t nextPowerOfTwo(uint32_t value, bool strict = true);
uint32_t prevPowerOfTwo(uint32_t value, bool strict = true);

static int modulo(int a, int b) {
    int m = a % b;
    if (m < 0) {
        m = (b < 0) ? m - b : m + b;
    }
    return m;
}

inline int roundDownDivide(int a, int b) {
    return a >= 0 ? (a / b) : (a - b + 1) / b;
}

inline float deg2rad(float deg) {
    return deg / 180.f * M_PI;
}

inline float rad2deg(float rad) {
    return rad * 180.f / M_PI;
}

template<typename T>
bool multipleBitsSet(T x) {
    return x & (x - 1);
}

template<typename T>
bool exactlyOneBitSet(T x) {
    return x && !multipleBitsSet(x);
}
