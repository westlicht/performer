#pragma once

#include "ShiftRegister.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput(ShiftRegister &shiftRegister);

    void init();

    inline void setGate(int index, bool value) {
        if (value) {
            _gates |= (1 << index);
        } else {
            _gates &= ~(1 << index);
        }

        _shiftRegister.write(2, _gates);
    }

private:
    ShiftRegister &_shiftRegister;
    uint8_t _gates = 0;
};
