#pragma once

#include <cstdint>

class System {
public:
    static void init();
    static void deinit();

    static void reset();

    static void startSysTick();
    static void stopSysTick();

    static void tick() { ++_ticks; }
    static uint32_t ticks() { return _ticks; }

private:
    static uint32_t _ticks;
};
