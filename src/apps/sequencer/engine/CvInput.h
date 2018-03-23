#pragma once

#include "Config.h"

#include "drivers/Adc.h"

#include <array>

class CvInput {
public:
    static constexpr int Channels = CONFIG_CV_INPUT_CHANNELS;

    CvInput(Adc &adc);

    void init();

    void update();

    float channel(int index) const {
        return _channels[index];
    }

private:
    Adc &_adc;

    std::array<float, Channels> _channels;
};
