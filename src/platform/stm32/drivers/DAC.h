#pragma once

#include <array>

class DAC {
public:
    typedef uint16_t Value;
    typedef std::array<Value, 8> Values;

    void init();

    Values &values() { return _values; }
    inline Value &operator()(int channel) { return _values[channel]; }

    void write(int channel);
    void write();

private:
    Values _values;
};
