#pragma once

#include <cstdint>

class GateOutput {
public:
    void init();

    inline void setGate(int index, bool value) {
        if (value) {
            _state |= (1 << index);
        } else {
            _state &= ~(1 << index);
        }
    }

private:
    uint8_t _state;
};
