#pragma once

#include "TargetConfig.h"
#include "EncoderEvent.h"
#include "MidiEvent.h"

#include <array>
#include <bitset>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <cstdint>
#include <cstring>

namespace sim {

namespace stream {

    template<typename T>
    static void write(T data, std::ostream &stream) {
        stream.write(reinterpret_cast<const char *>(&data), sizeof(T));
    }

    template<typename T>
    static void read(T &data, std::istream &stream) {
        stream.read(reinterpret_cast<char *>(&data), sizeof(T));
    }

    template<typename T>
    static T read(std::istream &stream) {
        T data;
        stream.read(reinterpret_cast<char *>(&data), sizeof(T));
        return data;
    }

} // namespace stream

template<typename T>
class StateTrace {
public:
    typedef std::pair<uint32_t, T> Item;

    const std::vector<Item> &items() const { return _items; }

    void write(uint32_t time, const T &state) {
        if (_items.empty()) {
            _items.emplace_back(time, state);
        } else if (time == _items.back().first) {
            _items.back().second = state;
        } else if (state != _items.back().second) {
            _items.emplace_back(time, state);
        }
    }

    void writeStream(std::ostream &stream) const {
        stream::write<uint32_t>(_items.size(), stream);
        for (const auto &item : _items) {
            stream::write(item, stream);
        }
    }

    void readStream(std::istream &stream) {
        _items.resize(stream::read<uint32_t>(stream));
        for (auto &item : _items) {
            stream::read(item, stream);
        }
    }

private:
    std::vector<Item> _items;
};

template<typename T>
class EventTrace {
public:
    typedef std::pair<uint32_t, T> Item;

    const std::vector<Item> &items() const { return _items; }

    void write(uint32_t time, const T &event) {
        _items.emplace_back(time, event);
    }

    void writeStream(std::ostream &stream) const {
        stream::write<uint32_t>(_items.size(), stream);
        for (const auto &item : _items) {
            stream::write(item, stream);
        }
    }

    void readStream(std::istream &stream) {
        _items.reserve(stream::read<uint32_t>(stream));
        for (auto &item : _items) {
            stream::read(item, stream);
        }
    }

private:
    std::vector<Item> _items;
};

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

    std::array<float, Count> state;

    AdcState() { state.fill(0.f); }

    void set(int channel, uint16_t value, float voltage) {
        if (channel >= 0 && channel < Count) {
            state[channel] = voltage;
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

    std::array<float, Count> state;

    DacState() { state.fill(0.f); }

    void set(int channel, uint16_t value, float voltage) {
        if (channel >= 0 && channel < Count) {
            state[channel] = voltage;
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
    static constexpr int Count = TargetConfig::LcdWidth * TargetConfig::LcdHeight;

    std::array<uint8_t, Count> state;

    LcdState() {}

    void set(uint8_t *frameBuffer) {
        std::memcpy(state.data(), frameBuffer, Count);
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

typedef StateTrace<ButtonState> ButtonTrace;
typedef StateTrace<AdcState> AdcTrace;
typedef StateTrace<DigitalInputState> DigitalInputTrace;
typedef StateTrace<LedState> LedTrace;
typedef StateTrace<GateOutputState> GateOutputTrace;
typedef StateTrace<DacState> DacTrace;
typedef StateTrace<DigitalOutputState> DigitalOutputTrace;
typedef StateTrace<LcdState> LcdTrace;

typedef EventTrace<EncoderEvent> EncoderTrace;
typedef EventTrace<MidiEvent> MidiTrace;

struct TargetTrace {
    // state traces
    ButtonTrace button;
    AdcTrace adc;
    DigitalInputTrace digitalInput;
    LedTrace led;
    GateOutputTrace gateOutput;
    DacTrace dac;
    DigitalOutputTrace digitalOutput;
    LcdTrace lcd;

    // event traces
    EncoderTrace encoder;
    MidiTrace midiInput;
    MidiTrace midiOutput;

    void writeStream(std::ostream &stream) const {
        button.writeStream(stream);
        adc.writeStream(stream);
        digitalInput.writeStream(stream);
        led.writeStream(stream);
        gateOutput.writeStream(stream);
        dac.writeStream(stream);
        digitalOutput.writeStream(stream);
        lcd.writeStream(stream);
        encoder.writeStream(stream);
        midiInput.writeStream(stream);
        midiOutput.writeStream(stream);
    }

    void readStream(std::istream &stream) {
        button.readStream(stream);
        adc.readStream(stream);
        digitalInput.readStream(stream);
        led.readStream(stream);
        gateOutput.readStream(stream);
        dac.readStream(stream);
        digitalOutput.readStream(stream);
        lcd.readStream(stream);
        encoder.readStream(stream);
        midiInput.readStream(stream);
        midiOutput.readStream(stream);
    }

    void saveToFile(const std::string &filename) const {
        std::ofstream ofs(filename, std::ios::binary);
        writeStream(ofs);
        ofs.close();
    }

    void loadFromFile(const std::string &filename) {
        std::ifstream ifs(filename, std::ios::binary);
        readStream(ifs);
        ifs.close();
    }

    void saveToText(const std::string &filename) const;
};

} // namespace sim
