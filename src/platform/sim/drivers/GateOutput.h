#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    inline void setGates(uint8_t gates) {
        for (int i = 0; i < 8; ++i) {
            _simulator.writeGate(i, (gates >> i) & 1);
        }
    }

    inline void setGate(int index, bool value) {
        _simulator.writeGate(index, value);
    }

private:
    sim::Simulator &_simulator;
};
