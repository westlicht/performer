#pragma once

#include "SystemConfig.h"

#include "sim/Simulator.h"
#include "sim/widgets/Rotary.h"

#include <cstdint>
#include <cstdlib>

class ADC {
public:
    static constexpr size_t Channels = CONFIG_ADC_CHANNELS;

    ADC() :
        _simulator(sim::Simulator::instance())
    {
        for (size_t i = 0; i < Channels; ++i) {
            auto rotary = _simulator.window().createWidget<sim::Rotary>(sim::Vector2i(50 + i * 50, 450), sim::Vector2i(40, 40));
            rotary->setValueCallback([this, i] (float value) {
                setChannel(i, value);
            });
            setChannel(i, 0.5f);
        }
    }

    void init() {}

    uint16_t channel(int index) const {
        return _channels[index];
    }

private:
    void setChannel(int index, float value) {
        _channels[index] = uint16_t(std::floor(0xffff * value));
    }

    sim::Simulator &_simulator;

    std::array<uint16_t, Channels> _channels;
};
