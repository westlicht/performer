#pragma once

#include "drivers/HighResolutionTimer.h"

class TapTempo {
public:
    TapTempo() {
        reset();
    }

    void reset() {
        _lastTime = 0;
        _lastInterval = 0;
    }

    float tap(float bpm) {
        uint32_t currentTime = HighResolutionTimer::us();
        uint32_t interval = currentTime - _lastTime;

        // reset averaging on first tap or if current interval is far off the last interval
        if (_lastTime == 0 || (interval < _lastInterval / 2 || interval > _lastInterval * 2)) {
            _intervalAverage.reset();
            _intervalAverage.push(uint32_t(60000000 / bpm));
        } else {
            _intervalAverage.push(interval);
            bpm = 60000000.f / _intervalAverage();
        }

        _lastTime = currentTime;
        _lastInterval = interval;

        return bpm;
    }

private:
    uint32_t _lastTime;
    uint32_t _lastInterval;
    MovingAverage<uint32_t, 8> _intervalAverage;
};
