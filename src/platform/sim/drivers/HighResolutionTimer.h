#pragma once

#include <cstdint>

class HighResolutionTimer {
public:
    static inline uint32_t us() {
        return 0;
    }

private:
    static uint32_t _ticks;
};
