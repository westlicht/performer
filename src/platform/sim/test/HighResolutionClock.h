#pragma once

#include <chrono>

#include <cstdint>

namespace HighResolutionClock {

    static std::chrono::time_point<std::chrono::high_resolution_clock> start;

    static void init() {
        start = std::chrono::high_resolution_clock::now();
    }

    static uint32_t us() {
        auto current = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(current - start)).count();
    }

};
