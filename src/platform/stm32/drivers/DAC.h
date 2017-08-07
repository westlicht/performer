#pragma once

#include <cstdint>

class DAC {
public:
    typedef uint16_t Value;

    void init();

    void setValue(int channel, Value value) {
        _values[channel] = value;
    }

    void write(int channel);
    void write();

private:
    Value _values[8];
};
