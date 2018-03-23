#pragma once

#include <cstdint>

class Types {
public:
    enum class RunMode : uint8_t {
        Forward,
        Backward,
        PingPong,
        Pendulum,
        Random,
        Last
    };

    static const char *runModeName(RunMode runMode) {
        switch (runMode) {
        case RunMode::Forward:  return "Forward";
        case RunMode::Backward: return "Backward";
        case RunMode::PingPong: return "PingPong";
        case RunMode::Pendulum: return "Pendulum";
        case RunMode::Random:   return "Random";
        case RunMode::Last:     break;
        }
        return nullptr;
    }

    enum class MidiPort : uint8_t {
        Midi,
        UsbMidi,
        Last
    };

    static const char *midiPortName(MidiPort midiPort) {
        switch (midiPort) {
        case MidiPort::Midi:    return "MIDI";
        case MidiPort::UsbMidi: return "USB";
        case MidiPort::Last:    break;
        }
        return nullptr;
    }


}; // namespace Types
