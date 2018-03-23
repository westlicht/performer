#pragma once

#include "Config.h"

#include "drivers/Dac.h"

#include <array>

class CvOutput {
public:
    static constexpr int Channels = CONFIG_CV_OUTPUT_CHANNELS;

    CvOutput(Dac &dac);

    void init();

    void update();

    float channel(int index) const {
        return _channels[index];
    }

    void setChannel(int index, float value) {
        _channels[index] = value;
    }

private:
    Dac &_dac;

    std::array<float, Channels> _channels;
};
