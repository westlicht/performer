#pragma once

#include "TargetState.h"
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
    typedef T Record;
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
    typedef T Record;
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
        _items.resize(stream::read<uint32_t>(stream));
        for (auto &item : _items) {
            stream::read(item, stream);
        }
    }

private:
    std::vector<Item> _items;
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

    void writeStream(std::ostream &stream) const;
    void readStream(std::istream &stream);

    void saveToFile(const std::string &filename) const;
    void loadFromFile(const std::string &filename);

    void saveToText(const std::string &filename) const;
};

} // namespace sim
