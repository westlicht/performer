#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class GateOutput {
public:
    GateOutput() :
        _simulator(sim::Simulator::instance())
    {}

    inline void setGate(int index, bool value) {
        _simulator.writeGate(index, value);
    }

private:
    sim::Simulator &_simulator;
};
