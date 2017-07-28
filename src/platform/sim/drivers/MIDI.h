#pragma once

#include "sim/Simulator.h"

#include <functional>

#include <cstdint>

class MIDI {
public:
    MIDI(sim::Simulator &simulator) :
        _simulator(simulator)
    {}

    // void send(uint8_t data);
    // bool recv()

    void setRecvFilter(std::function<bool(uint8_t)> filter) {}

private:
    sim::Simulator &_simulator;
};
