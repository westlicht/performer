#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    void update() {
        for (int i = 0; i < 8; ++i) {
            _simulator.writeGateOutput(i, (_gates >> i) & 1);
        }
    }

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
    sim::Simulator &_simulator;
    uint8_t _gates = 0;
};
