#pragma once

#include <cstdint>

class Timer {
public:
    Timer() {

    }

    void reset() {
        _startTime = currentTime();
    }

    uint32_t elapsed() {
        return currentTime() - _startTime;
    }


private:
    uint32_t currentTime() {
        return CURRENT_TIME();
    }

    uint32_t _startTime;
};
