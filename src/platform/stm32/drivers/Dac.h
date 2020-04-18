#pragma once

#include "SystemConfig.h"

#include <cstdint>
#include <cstdlib>

class Dac {
public:
    enum class Type {
        DAC8568C,
        DAC8568A
    };

    static constexpr int Channels = CONFIG_DAC_CHANNELS;

    typedef uint16_t Value;

    Dac(Type type = Type::DAC8568C);

    void init();

    void setValue(int channel, Value value) {
        _values[channel] = value;
    }

    void write(int channel);
    void write();

private:
    void writeDac(uint8_t command, uint8_t address, uint16_t data, uint8_t function);

    void reset();
    void setInternalRef(bool enabled);

    enum ClearCode {
        ClearZeroScale  = 0,
        ClearMidScale   = 1,
        ClearFullScale  = 2,
        ClearIgnore     = 3,
    };

    void setClearCode(ClearCode code);

    Value _values[Channels];
    uint32_t _dataShift = 0;
};
