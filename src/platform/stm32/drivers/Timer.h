#pragma once

#include <functional>

class Timer {
public:
    enum HardwareTimer {
        // HardwareTimer1,
        // HardwareTimer2,
        HardwareTimer3,
        HardwareTimer4,
        HardwareTimer5,
        // HardwareTimer6,
        HardwareTimerCount,
    };

    Timer(HardwareTimer hardwareTimer);

    void init();

    void reset();
    void enable();
    void disable();

    void setPeriod(uint32_t us);

    void setHandler(std::function<void()> handler);

private:
    HardwareTimer _hardwareTimer;
    std::function<void()> _handler;
};
