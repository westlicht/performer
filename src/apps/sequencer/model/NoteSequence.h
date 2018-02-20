#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"

#include "engine/Scale.h"

#include "core/math/Math.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class NoteSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<3> GateProbability;
    typedef UnsignedValue<3> Length;
    typedef SignedValue<4> LengthVariationRange;
    typedef UnsignedValue<3> LengthVariationProbability;
    typedef SignedValue<7> Note;
    typedef SignedValue<7> NoteVariationRange;
    typedef UnsignedValue<3> NoteVariationProbability;

    class Step {
    public:
        // gate

        bool gate() const { return _data0.gate ? true : false; }
        void setGate(bool gate) { _data0.gate = gate; }
        void toggleGate() { setGate(!gate()); }

        // gateProbability

        int gateProbability() const { return _data0.gateProbability; }
        void setGateProbability(int gateProbability) { _data0.gateProbability = gateProbability; }

        // length

        int length() const { return _data0.length; }
        void setLength(int length) { _data0.length = length; }

        // lengthVariationRange

        int lengthVariationRange() const { return LengthVariationRange::Min + _data0.lengthVariationRange; }
        void setLengthVariationRange(int lengthVariationRange) { _data0.lengthVariationRange = lengthVariationRange - LengthVariationRange::Min; }

        // lengthVariationProbability

        int lengthVariationProbability() const { return _data0.lengthVariationProbability; }
        void setLengthVariationProbability(int lengthVariationProbability) { _data0.lengthVariationProbability = lengthVariationProbability; }

        // note

        int note() const { return Note::Min + _data0.note; }
        void setNote(int note) { _data0.note = note - Note::Min; }

        // noteVariationRange

        int noteVariationRange() const { return NoteVariationRange::Min + _data0.noteVariationRange; }
        void setNoteVariationRange(int noteVariationRange) { _data0.noteVariationRange = noteVariationRange - NoteVariationRange::Min; }

        // noteVariationProbability

        int noteVariationProbability() const { return _data0.noteVariationProbability; }
        void setNoteVariationProbability(int noteVariationProbability) { _data0.noteVariationProbability = noteVariationProbability; }


        void clear();

        void write(WriteContext &context, int index) const;
        void read(ReadContext &context, int index);

    private:
        union {
            uint32_t raw;
            BitField<0, 1> gate;
            BitField<1, GateProbability::Bits> gateProbability;
            BitField<4, Length::Bits> length;
            BitField<7, LengthVariationRange::Bits> lengthVariationRange;
            BitField<11, LengthVariationProbability::Bits> lengthVariationProbability;
            BitField<14, Note::Bits> note;
            BitField<21, NoteVariationRange::Bits> noteVariationRange;
            BitField<28, NoteVariationProbability::Bits> noteVariationProbability;
            // 1 bit left
        } _data0;
        uint16_t _data1;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    enum class PlayMode : uint8_t {
        Forward,
        Backward,
        PingPong,
        Pendulum,
        Random,
        Last
    };

    static const char *playModeName(PlayMode playMode) {
        switch (playMode) {
        case PlayMode::Forward:     return "Forward";
        case PlayMode::Backward:    return "Backward";
        case PlayMode::PingPong:    return "PingPong";
        case PlayMode::Pendulum:    return "Pendulum";
        case PlayMode::Random:      return "Random";
        case PlayMode::Last:        break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // scale

    int scale() const { return _scale; }
    void setScale(int scale) {
        _scale = clamp(scale, 0, Scale::Count - 1);
    }

    // divisor

    int divisor() const { return _divisor; }
    void setDivisor(int divisor) {
        _divisor = clamp(divisor, 1, 128);
    }

    // resetMeasure

    int resetMeasure() const { return _resetMeasure; }
    void setResetMeasure(int resetMeasure) {
        _resetMeasure = clamp(resetMeasure, 0, 128);
    }

    // playMode

    PlayMode playMode() const { return _playMode; }
    void setPlayMode(PlayMode playMode) {
        _playMode = playMode;
    }

    // firstStep

    int firstStep() const { return _firstStep; }
    void setFirstStep(int firstStep) {
        _firstStep = clamp(firstStep, 0, lastStep());
    }

    // lastStep

    int lastStep() const { return _lastStep; }
    void setLastStep(int lastStep) {
        _lastStep = clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1);
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

    void setGates(std::initializer_list<int> gates);
    void setNotes(std::initializer_list<int> notes);

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    uint8_t _scale;
    uint8_t _divisor;
    uint8_t _resetMeasure;
    PlayMode _playMode;
    uint8_t _firstStep;
    uint8_t _lastStep;
    StepArray _steps;
};
