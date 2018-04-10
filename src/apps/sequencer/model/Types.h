#pragma once

#include "core/utils/StringBuilder.h"

#include <array>

#include <cstdint>

class Types {
public:
    // PlayMode

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

    // FillMode

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

    // RunMode

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

    // VoltageRange

    enum class VoltageRange : uint8_t {
        Unipolar1V,
        Unipolar2V,
        Unipolar3V,
        Unipolar4V,
        Unipolar5V,
        Bipolar1V,
        Bipolar2V,
        Bipolar3V,
        Bipolar4V,
        Bipolar5V,
        Last
    };

    static const char *voltageRangeName(VoltageRange voltageRange) {
        switch (voltageRange) {
        case VoltageRange::Unipolar1V:  return "1V Unipolar";
        case VoltageRange::Unipolar2V:  return "2V Unipolar";
        case VoltageRange::Unipolar3V:  return "3V Unipolar";
        case VoltageRange::Unipolar4V:  return "4V Unipolar";
        case VoltageRange::Unipolar5V:  return "5V Unipolar";
        case VoltageRange::Bipolar1V:   return "1V Bipolar";
        case VoltageRange::Bipolar2V:   return "2V Bipolar";
        case VoltageRange::Bipolar3V:   return "3V Bipolar";
        case VoltageRange::Bipolar4V:   return "4V Bipolar";
        case VoltageRange::Bipolar5V:   return "5V Bipolar";
        case VoltageRange::Last:        break;
        }
        return nullptr;
    }

    static std::array<float, 2> voltageRangeValues(VoltageRange voltageRange) {
        switch (voltageRange) {
        case VoltageRange::Unipolar1V:  return {{ 0.f, 1.f }};
        case VoltageRange::Unipolar2V:  return {{ 0.f, 2.f }};
        case VoltageRange::Unipolar3V:  return {{ 0.f, 3.f }};
        case VoltageRange::Unipolar4V:  return {{ 0.f, 4.f }};
        case VoltageRange::Unipolar5V:  return {{ 0.f, 5.f }};
        case VoltageRange::Bipolar1V:   return {{ -1.f, 1.f }};
        case VoltageRange::Bipolar2V:   return {{ -2.f, 2.f }};
        case VoltageRange::Bipolar3V:   return {{ -3.f, 3.f }};
        case VoltageRange::Bipolar4V:   return {{ -4.f, 4.f }};
        case VoltageRange::Bipolar5V:   return {{ -5.f, 5.f }};
        case VoltageRange::Last:        break;
        }
        return {{ 0.f, 0.f }};
    }

    // MidiPort

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

    // Utilities
    // TODO maybe move these

    static void printMidiChannel(StringBuilder &str, int midiChannel) {
        if (midiChannel == -1) {
            str("Omni");
        } else {
            str("%d", midiChannel + 1);
        }
    }

    static void printNote(StringBuilder &str, int note) {
        static const char *names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        str(names[note]);
    }

    static void printMidiNote(StringBuilder &str, int midiNote) {
        printNote(str, midiNote % 12);
        int octave = midiNote / 12 - 1;
        str("%d", octave);
    }


}; // namespace Types
