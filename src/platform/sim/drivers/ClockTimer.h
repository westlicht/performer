#pragma once

#include "sim/Simulator.h"

#include <functional>

#include <cstdint>

class ClockTimer {
public:
    ClockTimer(sim::Simulator &simulator) :
        _simulator(simulator)
    {
        _simulator.addUpdateCallback([this] () { update(); });
    }

    void reset() {
        disable();
    }

    void enable() {
        _enabled = true;
        _lastTicks = _simulator.ticks();
    }

    void disable() {
        _enabled = false;
    }

    void setPeriod(uint32_t us) { _period = us * 0.001; }

    void setHandler(std::function<void()> handler) { _handler = handler; }

private:
    void update() {
        if (!_enabled) {
            return;
        }
        double ticks = _simulator.ticks();
        while (ticks - _lastTicks >= _period) {
            _lastTicks += _period;
            if (_handler) {
                _handler();
            }
        }
    }

    sim::Simulator &_simulator;
    double _period = 0.0;
    std::function<void()> _handler;
    bool _enabled = false;
    double _lastTicks;
};
