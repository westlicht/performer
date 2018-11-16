#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    inline uint8_t gates() const { return _gates; }

    inline void setGates(uint8_t gates) {
        _gates = gates;
        for (int i = 0; i < 8; ++i) {
            _simulator.writeGateOutput(i, (gates >> i) & 1);
        }
    }

    inline void setGate(int index, bool value) {
        _gates = value ? (_gates | (1 << index)) : (_gates & ~(1 << index));
        _simulator.writeGateOutput(index, value);
    }

private:
    sim::Simulator &_simulator;
    uint8_t _gates = 0;
};
