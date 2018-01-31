#pragma once

#include "SystemConfig.h"

#include <cstdint>
#include <cstdlib>

class ADC {
public:
    static constexpr size_t Channels = CONFIG_ADC_CHANNELS;

    void init();

    uint16_t channel(int index) const {
        return _channels[index];
    }

private:
    uint16_t _channels[Channels];
};
