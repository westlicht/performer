#pragma once

#include "drivers/HighResolutionTimer.h"

class TapTempo {
public:
    TapTempo() {
        reset(120.f);
    }

    float bpm() const { return _bpm; }

    void reset(float bpm) {
        _bpm = bpm;
        _lastTime = 0;
        _intervalAverage.reset();
        _intervalAverage.push(uint32_t(60000000 / bpm));
    }

    void tap() {
        uint32_t currentTime = HighResolutionTimer::us();

        if (_lastTime) {
            _intervalAverage.push(currentTime - _lastTime);
            _bpm = 60000000.f / _intervalAverage();
        }

        _lastTime = currentTime;
    }

private:
    float _bpm;
    uint32_t _lastTime;
    MovingAverage<uint32_t, 8> _intervalAverage;
};
