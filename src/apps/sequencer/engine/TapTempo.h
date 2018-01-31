#pragma once

#include "core/Debug.h"

#include "drivers/HighResolutionTimer.h"

class TapTempo {
public:
    TapTempo() {
        reset();
    }

    float bpm() const { return _bpm; }

    void reset() {
        _bpm = 0.f;
        _lastTime = 0;
        _intervalAverage.reset();
    }

    bool tap() {
        bool result = false;
        uint32_t currentTime = HighResolutionTimer::us();

        if (_lastTime) {
            _intervalAverage.push(currentTime - _lastTime);
            _lastTime = currentTime;

            _bpm = 60.f / (_intervalAverage() / 1000000.f);
            result = true;
        }

        _lastTime = currentTime;

        return result;
    }

private:
    float _bpm;
    uint32_t _lastTime;
    MovingAverage<uint32_t, 8> _intervalAverage;
};
