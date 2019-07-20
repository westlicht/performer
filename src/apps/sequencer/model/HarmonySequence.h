#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "Scale.h"
#include "Chord.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class HarmonySequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<7> Length;

    class Step {
    public:
        // length

        int length() const { return _data0.length + 1; }
        void setLength(int length) {
            _data0.length = Length::clamp(length - 1);
        }

        void editLength(int value, bool shift) {
            setLength(length() + value);
        }

        void printLength(StringBuilder &str) const {
            str("%d", length());
        }

        // chordRoot

        int chordRoot() const { return _data1.chordRoot; }
        void setChordRoot(int chordRoot) {
            _data1.chordRoot = clamp(chordRoot, 0, 11);
        }

        void editChordRoot(int value, bool shift) {
            setChordRoot(chordRoot() + value);
        }

        void printChordRoot(StringBuilder &str) const {
            Types::printNote(str, chordRoot());
        }

        // chordQuality

        Chord::Quality chordQuality() const { return Chord::Quality(int(_data1.chordQuality)); }
        void setChordQuality(Chord::Quality chordQuality) {
            _data1.chordQuality = int(chordQuality);
        }

        void editChordQuality(int value, bool shift) {
            setChordQuality(ModelUtils::adjustedEnum(chordQuality(), value));
        }

        void printChordQuality(StringBuilder &str) const {
            str(Chord::qualityName(chordQuality()));
        }

        // chordVoicing

        Chord::Voicing chordVoicing() const { return Chord::Voicing(int(_data1.chordVoicing)); }
        void setChordVoicing(Chord::Voicing chordVoicing) {
            _data1.chordVoicing = int(chordVoicing);
        }

        void editChordVoicing(int value, bool shift) {
            setChordVoicing(ModelUtils::adjustedEnum(chordVoicing(), value));
        }

        void printChordVoicing(StringBuilder &str) const {
            str(Chord::voicingName(chordVoicing()));
        }


        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        union {
            uint32_t raw;
            BitField<uint32_t, 0, Length::Bits> length;
            // 25 bits left
        } _data0;
        union {
            uint32_t raw;
            BitField<uint32_t, 0, 4> chordRoot;
            BitField<uint32_t, 4, 5> chordQuality;
            BitField<uint32_t, 9, 3> chordVoicing;
            // 20 bits left
        } _data1;
    };

    static constexpr int StepCount = 16;

    typedef std::array<Step, StepCount> StepArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // divisor

    int divisor() const { return _divisor; }
    void setDivisor(int divisor) {
        _divisor = clamp(divisor, 1, 192);
    }

    int indexedDivisor() const { return ModelUtils::divisorToIndex(divisor()); }
    void setIndexedDivisor(int index) {
        int divisor = ModelUtils::indexToDivisor(index);
        if (divisor > 0) {
            setDivisor(divisor);
        }
    }

    void editDivisor(int value, bool shift) {
        setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
    }

    void printDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, divisor());
    }

    // resetMeasure

    int resetMeasure() const { return _resetMeasure; }
    void setResetMeasure(int resetMeasure) {
        _resetMeasure = clamp(resetMeasure, 0, 128);
    }

    void editResetMeasure(int value, bool shift) {
        setResetMeasure(ModelUtils::adjustedByPowerOfTwo(resetMeasure(), value, shift));
    }

    void printResetMeasure(StringBuilder &str) const {
        if (resetMeasure() == 0) {
            str("off");
        } else {
            str("%d", resetMeasure());
        }
    }

    // runMode

    Types::RunMode runMode() const { return _runMode; }
    void setRunMode(Types::RunMode runMode) {
        _runMode = ModelUtils::clampedEnum(runMode);
    }

    void editRunMode(int value, bool shift) {
        setRunMode(ModelUtils::adjustedEnum(runMode(), value));
    }

    void printRunMode(StringBuilder &str) const {
        str(Types::runModeName(runMode()));
    }

    // firstStep

    int firstStep() const { return _firstStep; }
    void setFirstStep(int firstStep) {
        _firstStep = clamp(firstStep, 0, lastStep());
    }

    void editFirstStep(int value, bool shift) {
        setFirstStep(firstStep() + value);
    }

    void printFirstStep(StringBuilder &str) const {
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const { return _lastStep; }
    void setLastStep(int lastStep) {
        _lastStep = clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1);
    }

    void editLastStep(int value, bool shift) {
        setLastStep(lastStep() + value);
    }

    void printLastStep(StringBuilder &str) const {
        str("%d", lastStep() + 1);
    }

    // steps

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();
    void clearSteps();

    void shiftSteps(int direction);

    void duplicateSteps();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    int8_t _trackIndex = -1;
    uint8_t _divisor;
    uint8_t _resetMeasure;
    Types::RunMode _runMode;
    uint8_t _firstStep;
    uint8_t _lastStep;
    StepArray _steps;

    friend class HarmonyTrack;
};
