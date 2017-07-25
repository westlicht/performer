#pragma once

#include <cstdint>

class System {
    static volatile uint32_t _ticks;

public:
    static void init();
    static uint32_t ticks() { return _ticks; }

    static void tick();
};
