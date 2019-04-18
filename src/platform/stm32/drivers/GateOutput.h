#pragma once

#include "ShiftRegister.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput(ShiftRegister &shiftRegister);

    void init();

    void update();

    inline uint8_t gates() const { return _gates; }

    inline void setGates(uint8_t gates) {
        _gates = gates;
    }

    inline void setGate(int index, bool value) {
        if (value) {
            _gates |= (1 << index);
        } else {
            _gates &= ~(1 << index);
        }
    }

private:
    ShiftRegister &_shiftRegister;
    uint8_t _gates = 0;
};
