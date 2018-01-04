#pragma once

#include <cstdint>

class ADC {
public:
    static const int Channels = 4;

    void init();

    uint16_t channel(int index) const {
        return _channels[index];
    }

private:
    uint16_t _channels[Channels];
};
