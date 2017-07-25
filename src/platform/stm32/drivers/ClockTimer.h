#pragma once

#include <functional>

#include <cstdint>

class ClockTimer {
public:
    void init();

    void reset();
    void enable();
    void disable();

    void setPeriod(uint32_t us);

    void setHandler(std::function<void()> handler);
};
