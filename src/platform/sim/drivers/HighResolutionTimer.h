#pragma once

#include <chrono>

#include <cstdint>

namespace detail {
    static std::chrono::time_point<std::chrono::high_resolution_clock> start;
}

class HighResolutionTimer {
public:
    static void init() {
        detail::start = std::chrono::high_resolution_clock::now();
    }

    static uint32_t us() {
        auto current = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(current - detail::start)).count();
    }

};
