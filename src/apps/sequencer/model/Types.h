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
        Gates,
        NextPattern,
        Last
    };

    static const char *fillModeName(FillMode fillMode) {
        switch (fillMode) {
        case FillMode::None:        return "None";
        case FillMode::Gates:       return "Gates";
        case FillMode::NextPattern: return "Next Pattern";
        case FillMode::Last:        break;
        }
        return nullptr;
    }

    enum class RunMode : uint8_t {
        Forward,
        Backward,
        PingPong,
        Pendulum,
        Random,
        RandomWalk,
        Last
    };

    static const char *runModeName(RunMode runMode) {
        switch (runMode) {
        case RunMode::Forward:      return "Forward";
        case RunMode::Backward:     return "Backward";
        case RunMode::PingPong:     return "PingPong";
        case RunMode::Pendulum:     return "Pendulum";
        case RunMode::Random:       return "Random";
        case RunMode::RandomWalk:   return "Random Walk";
        case RunMode::Last:         break;
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

    static void printMidiNote(StringBuilder &str, int note) {
        static const char *names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int octave = note / 12 - 1;
        str("%s%d", names[note % 12], octave);
    }


}; // namespace Types
