#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class ClockTimer {
public:
    struct Listener {
        virtual void onClockTimerTick() = 0;
    };

    ClockTimer() :
        _simulator(sim::Simulator::instance())
    {
        _simulator.addUpdateCallback([this] () { update(); });
    }

    void init() {
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

    uint32_t period() const {
        return _period;
    }

    void setPeriod(uint32_t us) {
        _period = us;
        _periodTicks = us * 0.001;
    }

    void setListener(Listener *listener) {
        _listener = listener;
    }

private:
    void update() {
        if (!_enabled) {
            return;
        }
        double ticks = _simulator.ticks();
        while (ticks - _lastTicks >= _periodTicks) {
            _lastTicks += _periodTicks;
            if (_listener) {
                _listener->onClockTimerTick();
            }
        }
    }

    sim::Simulator &_simulator;
    uint32_t _period = 0;
    double _periodTicks = 0.0;
    Listener *_listener = nullptr;
    bool _enabled = false;
    double _lastTicks;
};
