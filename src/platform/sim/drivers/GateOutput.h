#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput(sim::Simulator &simulator) :
        _simulator(simulator)
    {}

    inline void setGate(int index, bool value) {
        _simulator.writeGate(index, value);
    }

private:
    sim::Simulator &_simulator;
};
