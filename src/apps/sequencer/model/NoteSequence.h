#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class NoteSequence {
public:
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

        // defaults

        void setDefault() {
            setGate(false);
            setGateProbability(GateProbability::Max);
            setLength(Length::Max / 2);
            setLengthVariationRange(0);
            setLengthVariationProbability(0);
            setNote(0);
        }

        // Serialization

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
    };

    // parameters

    int scale() const { return _scale; }
    void setScale(int scale) { _scale = scale; }

    // steps

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    // utility functions

    void setDefault() {
        setScale(3);
        for (auto &step : _steps) {
            step.setDefault();
        }
    }

    void setGates(std::initializer_list<int> gates) {
        size_t step = 0;
        for (auto gate : gates) {
            if (step < _steps.size()) {
                _steps[step++].setGate(gate);
            }
        }
    }

    void setNotes(std::initializer_list<int> notes) {
        size_t step = 0;
        for (auto note : notes) {
            if (step < _steps.size()) {
                _steps[step++].setNote(note);
            }
        }
    }

    // Serialization

    void write(WriteContext &context, int index) const;
    void read(ReadContext &context, int index);

private:
    uint8_t _scale;
    StepArray _steps;
};
