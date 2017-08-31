#pragma once

#include <cstdint>

class GateOutput {
public:
    void init();

    inline void setGate(int index, bool value) {
        if (value) {
            _gates |= (1 << index);
        } else {
            _gates &= ~(1 << index);
        }
    }

    uint8_t &gates() { return _gates; }

private:
    uint8_t _gates = 0;
};
