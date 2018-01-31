#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"

#include <cstdint>
#include <cstdlib>

class ADC {
public:
    static constexpr size_t Channels = CONFIG_ADC_CHANNELS;

    ADC() :
        _simulator(sim::Simulator::instance())
    {}

    void init() {}

    uint16_t channel(int index) const {
        return _simulator.readADC(index);
    }

private:
    sim::Simulator &_simulator;
};
