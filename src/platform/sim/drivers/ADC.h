#pragma once

#include "sim/Simulator.h"

#include <cstdint>

class ADC {
public:
    static const int Channels = 4;

    ADC(sim::Simulator &simulator) :
        _simulator(simulator)
    {
    }

    uint16_t channel(int index) const {
        return _simulator.readADC(index);
    }

private:
    sim::Simulator &_simulator;
};
