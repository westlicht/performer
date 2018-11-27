#pragma once

#include "TargetConfig.h"
#include "FrameBuffer.h"

#include <bitset>
#include <array>

#include <cstdint>

namespace sim {

struct ButtonState {
    static constexpr int Count = TargetConfig::Rows * TargetConfig::ColsButton;

    std::bitset<Count> state;

    ButtonState() : state(false) {}

    void set(int index, bool pressed) {
        if (index >= 0 && index < Count) {
            state.set(index, pressed);
        }
    }

    bool operator==(const ButtonState &other) const { return state == other.state; }
    bool operator!=(const ButtonState &other) const { return state != other.state; }
};

struct AdcState {
    static constexpr int Count = TargetConfig::AdcChannels;

    std::array<uint16_t, Count> state;

    AdcState() { state.fill(0); }

    void set(int channel, uint16_t value) {
        if (channel >= 0 && channel < Count) {
            state[channel] = value;
        }
    }

    bool operator==(const AdcState &other) const { return state == other.state; }
    bool operator!=(const AdcState &other) const { return state != other.state; }
};

struct DigitalInputState {
    static constexpr int Count = TargetConfig::DigitalInputs;

    std::bitset<Count> state;

    DigitalInputState() : state(false) {}

    void set(int pin, bool high) {
        if (pin >= 0 && pin < Count) {
            state.set(pin, high);
        }
    }

    bool operator==(const DigitalInputState &other) const { return state == other.state; }
    bool operator!=(const DigitalInputState &other) const { return state != other.state; }
};

struct LedState {
    static constexpr int Count = TargetConfig::Rows * TargetConfig::ColsLed;

    std::bitset<Count * 2> state;

    LedState() : state(false) {}

    void set(int index, bool red, bool green) {
        if (index >= 0 && index < Count) {
            state.set(index * 2, red);
            state.set(index * 2 + 1, green);
        }
    }

    bool operator==(const LedState &other) const { return state == other.state; }
    bool operator!=(const LedState &other) const { return state != other.state; }
};

struct GateOutputState {
    static constexpr int Count = TargetConfig::GateChannels;

    std::bitset<Count> state;

    GateOutputState() : state(false) {}

    void set(int channel, bool high) {
        if (channel >= 0 && channel < Count) {
            state.set(channel, high);
        }
    }

    bool operator==(const GateOutputState &other) const { return state == other.state; }
    bool operator!=(const GateOutputState &other) const { return state != other.state; }
};

struct DacState {
    static constexpr int Count = TargetConfig::DacChannels;

    std::array<uint16_t, Count> state;

    DacState() { state.fill(0); }

    void set(int channel, uint16_t value) {
        if (channel >= 0 && channel < Count) {
            state[channel] = value;
        }
    }

    bool operator==(const DacState &other) const { return state == other.state; }
    bool operator!=(const DacState &other) const { return state != other.state; }
};

struct DigitalOutputState {
    static constexpr int Count = TargetConfig::DigitalOutputs;

    std::bitset<Count> state;

    DigitalOutputState() : state(false) {}

    void set(int pin, bool high) {
        if (pin >= 0 && pin < Count) {
            state.set(pin, high);
        }
    }

    bool operator==(const DigitalOutputState &other) const { return state == other.state; }
    bool operator!=(const DigitalOutputState &other) const { return state != other.state; }
};

struct LcdState {
    FrameBuffer state;

    LcdState() {
        state.fill(0);
    }

    void set(const FrameBuffer &frameBuffer) {
        state = frameBuffer;
    }

    bool operator==(const LcdState &other) const { return state == other.state; }
    bool operator!=(const LcdState &other) const { return state != other.state; }
};

struct TargetState {
    ButtonState button;
    AdcState adc;
    DigitalInputState digitalInput;
    LedState led;
    GateOutputState gateOutput;
    DacState dac;
    DigitalOutputState digitalOutput;
    LcdState lcd;
};

} // namespace sim
