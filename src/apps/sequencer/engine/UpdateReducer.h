#pragma once

#include "os/os.h"

#include <cstdint>

template<uint32_t Interval>
class UpdateReducer {
public:
    bool update() {
        uint32_t currentTick = os::ticks();
        if (currentTick >= _lastUpdate + Interval) {
            _lastUpdate = currentTick;
            return true;
        }
        return false;
    }

private:
    uint32_t _lastUpdate = 0;
};
