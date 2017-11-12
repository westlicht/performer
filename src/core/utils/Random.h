#pragma once

#include <cstdint>

class Random {
public:
    Random(uint32_t seed = 0) :
        _state(seed)
    {}

    inline uint32_t next() {
        _state = _state * 1664525L + 1013904223L;
        return _state;
    }

    inline bool nextBinary() {
        return next() < 0x80000000;
    }

    inline uint32_t nextRange(uint32_t range) {
        return next() / (0xffffffff / range);
    }

private:
    uint32_t _state;
};
