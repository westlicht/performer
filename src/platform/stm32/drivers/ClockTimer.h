#pragma once

#include <cstdint>

class ClockTimer {
public:
    struct Listener {
        virtual void onClockTimerTick() = 0;
    };

    void init();

    void reset();
    void enable();
    void disable();

    uint32_t period() const { return _period; }
    void setPeriod(uint32_t us);

    void setListener(Listener *listener);

private:
    uint32_t _period = 0;
};
