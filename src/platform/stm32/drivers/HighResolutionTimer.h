#pragma once

#include <cstdint>

class HighResolutionTimer {
public:
    static void init();

    static inline uint32_t us() {
        return _ticks;
    }

    static inline void tick() {
        ++_ticks;
    }

private:
    static volatile uint32_t _ticks;
};
