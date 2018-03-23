#pragma once

#include "core/utils/StringBuilder.h"

#include <cstdint>

class Types {
public:
    enum class PlayMode : uint8_t {
        Free,
        Aligned,
        Last
    };

    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Free:    return "Free";
        case PlayMode::Aligned: return "Aligned";
        case PlayMode::Last:    break;
        }
        return nullptr;
    }

    enum class FillMode : uint8_t {
        None,
        // all gates
        // max gate probability
        Last
    };

    static const char *fillModeName(FillMode fillMode) {
        switch (fillMode) {
        case FillMode::None:    return "None";
        case FillMode::Last:    break;
        }
        return nullptr;
    }

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

    static void printMidiChannel(StringBuilder &str, int midiChannel) {
        if (midiChannel == -1) {
            str("Omni");
        } else {
            str("%d", midiChannel + 1);
        }
    }


}; // namespace Types
