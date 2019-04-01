#pragma once

#include "core/utils/StringBuilder.h"

#include <array>

#include <cstdint>

class Types {
public:
    // RecordMode

    enum class RecordMode : uint8_t {
        Overdub,
        Overwrite,
        StepRecord,
        Last
    };

    static const char *recordModeName(RecordMode recordMode) {
        switch (recordMode) {
        case RecordMode::Overdub:   return "Overdub";
        case RecordMode::Overwrite: return "Overwrite";
        case RecordMode::StepRecord:return "Step Record";
        case RecordMode::Last:      break;
        }
        return nullptr;
    }

    // PlayMode

    enum class PlayMode : uint8_t {
        Aligned,
        Free,
        Last
    };

    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Aligned: return "Aligned";
        case PlayMode::Free:    return "Free";
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
        Pendulum,
        PingPong,
        Random,
        RandomWalk,
        Last
    };

    static const char *runModeName(RunMode runMode) {
        switch (runMode) {
        case RunMode::Forward:      return "Forward";
        case RunMode::Backward:     return "Backward";
        case RunMode::Pendulum:     return "Pendulum";
        case RunMode::PingPong:     return "PingPong";
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

    struct VoltageRangeInfo {
        float lo;
        float hi;
    };

    static const VoltageRangeInfo *voltageRangeInfo(VoltageRange voltageRange) {
        return &voltageRangeInfos[int(voltageRange)];
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

    // Misc types

    struct LayerRange {
        int min;
        int max;
    };

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

private:
    static const VoltageRangeInfo voltageRangeInfos[];

}; // namespace Types
