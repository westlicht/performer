#pragma once

#include "sim/Simulator.h"

namespace sim {

class ClockSource {
public:
    ClockSource(Simulator &simulator, std::function<void()> handler) :
        _simulator(simulator),
        _handler(handler)
    {
        _simulator.addUpdateCallback([this] () { update(); });
    }

    void toggle() {
        _active = !_active;
        if (_active) {
            _lastTicks = _simulator.ticks();
        }
    }

    void update() {
        if (_active) {
            double currentTicks = _simulator.ticks();
            double interval = clockInterval() * 1000.0;
            while (_lastTicks <= currentTicks + interval) {
                if (_handler) {
                    _handler();
                }
                _lastTicks += interval;
            }
        }
    }

private:
    double clockInterval() {
        return 60.0 / (_bpm * _ppqn);
    }

    Simulator &_simulator;
    std::function<void()> _handler;

    bool _active = false;
    int _ppqn = 16;
    double _bpm = 120.0;

    double _lastTicks;
};

} // namespace sim
