#pragma once

#include "Config.h"
#include "Bitfield.h"
#include "Serialize.h"
#include "ModelUtils.h"
#include "Types.h"
#include "Scale.h"
#include "Routing.h"

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <array>
#include <cstdint>
#include <initializer_list>

class StageSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<4> Length;
    typedef SignedValue<7> Note;
    typedef UnsignedValue<4> GateCount;

    // gates:
    // off (0)
    // first (1)
    // all (=length)
    // hold
    // other count 1, 2, 3, 4, 5, 6

    // per step
    // - length (4bit)
    // - 12 bits left

    // per layer:
    // - note (7bit)
    // - gate (5bit)
    // - slide (1bit)
    // - 3 bits left

    enum class Layer {
        Length,
        Note0,
        Note1,
        GateCount0,
        GateCount1,
        // Gate,
        // GateProbability,
        // GateOffset,
        // Slide,
        // Retrigger,
        // RetriggerProbability,
        // Length,
        // LengthVariationRange,
        // LengthVariationProbability,
        // Note,
        // NoteVariationRange,
        // NoteVariationProbability,
        Last
    };

    static const char *layerName(Layer layer) {
        switch (layer) {
        case Layer::Length:                     return "LENGTH";
        // case Layer::Gate:                       return "GATE";
        // case Layer::GateProbability:            return "GATE PROB";
        // case Layer::GateOffset:                 return "GATE OFFSET";
        // case Layer::Slide:                      return "SLIDE";
        // case Layer::Retrigger:                  return "RETRIG";
        // case Layer::RetriggerProbability:       return "RETRIG PROB";
        // case Layer::Length:                     return "LENGTH";
        // case Layer::LengthVariationRange:       return "LENGTH RANGE";
        // case Layer::LengthVariationProbability: return "LENGTH PROB";
        case Layer::Note0:                      return "NOTE 1";
        case Layer::Note1:                      return "NOTE 2";
        case Layer::GateCount0:                 return "GATES 1";
        case Layer::GateCount1:                 return "GATES 2";
        // case Layer::NoteVariationRange:         return "NOTE RANGE";
        // case Layer::NoteVariationProbability:   return "NOTE PROB";
        case Layer::Last:                       break;
        }
        return nullptr;
    }

    static Types::LayerRange layerRange(Layer layer);

    class Step {
    public:
        //----------------------------------------
        // Properties
        //----------------------------------------

        // length

        int length() const { return _data0.length + 1; }
        void setLength(int length) {
            _data0.length = Length::clamp(length - 1);
        }

        // note

        int note(int channel) const { return Note::Min + _dataChannel[channel].note; }
        void setNote(int channel, int note) {
            _dataChannel[channel].note = Note::clamp(note) - Note::Min;
        }

        // gateCount

        int gateCount(int channel) const { return _dataChannel[channel].gateCount; }
        void setGateCount(int channel, int gateCount) {
            _dataChannel[channel].gateCount = GateCount::clamp(gateCount);
        }

#if 0
        // gate

        bool gate() const { return _data0.gate ? true : false; }
        void setGate(bool gate) { _data0.gate = gate; }
        void toggleGate() { setGate(!gate()); }

        // gateProbability

        int gateProbability() const { return _data0.gateProbability; }
        void setGateProbability(int gateProbability) {
            _data0.gateProbability = GateProbability::clamp(gateProbability);
        }

        // gateOffset

        int gateOffset() const { return GateOffset::Min + _data1.gateOffset; }
        void setGateOffset(int gateOffset) {
            // TODO: allow negative gate delay in the future
            _data1.gateOffset = std::max(0, GateOffset::clamp(gateOffset)) - GateOffset::Min;
        }

        // slide

        bool slide() const { return _data0.slide ? true : false; }
        void setSlide(bool slide) {
            _data0.slide = slide;
        }
        void toggleSlide() {
            setSlide(!slide());
        }

        // retrigger

        int retrigger() const { return _data1.retrigger; }
        void setRetrigger(int retrigger) {
            _data1.retrigger = Retrigger::clamp(retrigger);
        }

        // retriggerProbability

        int retriggerProbability() const { return _data1.retriggerProbability; }
        void setRetriggerProbability(int retriggerProbability) {
            _data1.retriggerProbability = RetriggerProbability::clamp(retriggerProbability);
        }

        // length

        int length() const { return _data0.length; }
        void setLength(int length) {
            _data0.length = Length::clamp(length);
        }

        // lengthVariationRange

        int lengthVariationRange() const { return LengthVariationRange::Min + _data0.lengthVariationRange; }
        void setLengthVariationRange(int lengthVariationRange) {
            _data0.lengthVariationRange = LengthVariationRange::clamp(lengthVariationRange) - LengthVariationRange::Min;
        }

        // lengthVariationProbability

        int lengthVariationProbability() const { return _data0.lengthVariationProbability; }
        void setLengthVariationProbability(int lengthVariationProbability) {
            _data0.lengthVariationProbability = LengthVariationProbability::clamp(lengthVariationProbability);
        }

        // note

        int note() const { return Note::Min + _data0.note; }
        void setNote(int note) {
            _data0.note = Note::clamp(note) - Note::Min;
        }

        // noteVariationRange

        int noteVariationRange() const { return NoteVariationRange::Min + _data0.noteVariationRange; }
        void setNoteVariationRange(int noteVariationRange) {
            _data0.noteVariationRange = NoteVariationRange::clamp(noteVariationRange) - NoteVariationRange::Min;
        }

        // noteVariationProbability

        int noteVariationProbability() const { return _data0.noteVariationProbability; }
        void setNoteVariationProbability(int noteVariationProbability) {
            _data0.noteVariationProbability = NoteVariationProbability::clamp(noteVariationProbability);
        }
#endif
        int layerValue(Layer layer) const;
        void setLayerValue(Layer layer, int value);

        //----------------------------------------
        // Methods
        //----------------------------------------

        Step() { clear(); }

        void clear();

        void write(VersionedSerializedWriter &writer) const;
        void read(VersionedSerializedReader &reader);

        bool operator==(const Step &other) const {
            return _data0.raw == other._data0.raw && _dataChannel[0].raw == other._dataChannel[0].raw && _dataChannel[1].raw == other._dataChannel[1].raw;
        }

        bool operator!=(const Step &other) const {
            return !(*this == other);
        }

    private:
        union {
            uint16_t raw;
            BitField<uint16_t, 0, 4> length;
            // BitField<uint32_t, 0, 1> gate;
            // BitField<uint32_t, 1, 1> slide;
            // BitField<uint32_t, 2, GateProbability::Bits> gateProbability;
            // BitField<uint32_t, 5, Length::Bits> length;
            // BitField<uint32_t, 8, LengthVariationRange::Bits> lengthVariationRange;
            // BitField<uint32_t, 12, LengthVariationProbability::Bits> lengthVariationProbability;
            // BitField<uint32_t, 15, Note::Bits> note;
            // BitField<uint32_t, 22, NoteVariationRange::Bits> noteVariationRange;
            // BitField<uint32_t, 29, NoteVariationProbability::Bits> noteVariationProbability;
        } _data0;
        union {
            uint16_t raw;
            BitField<uint16_t, 0, Note::Bits> note;
            BitField<uint16_t, 7, GateCount::Bits> gateCount;
        } _dataChannel[2];
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // scale

    int scale() const { return _scale; }
    void setScale(int scale) {
        _scale = clamp(scale, -1, Scale::Count - 1);
    }

    int indexedScale() const { return _scale + 1; }
    void setIndexedScale(int index) {
        setScale(index - 1);
    }

    void editScale(int value, bool shift) {
        setScale(scale() + value);
    }

    void printScale(StringBuilder &str) const {
        str(scale() < 0 ? "Default" : Scale::name(scale()));
    }

    const Scale &selectedScale(int defaultScale) const {
        return Scale::get(scale() < 0 ? defaultScale : scale());
    }

    // rootNote

    int rootNote() const { return _rootNote; }
    void setRootNote(int rootNote) {
        _rootNote = clamp(rootNote, -1, 11);
    }

    int indexedRootNote() const { return _rootNote + 1; }
    void setIndexedRootNote(int index) {
        setRootNote(index - 1);
    }

    void editRootNote(int value, bool shift) {
        setRootNote(rootNote() + value);
    }

    void printRootNote(StringBuilder &str) const {
        if (rootNote() < 0) {
            str("Default");
        } else {
            Types::printNote(str, rootNote());
        }
    }

    int selectedRootNote(int defaultRootNote) const {
        return rootNote() < 0 ? defaultRootNote : rootNote();
    }

    // divisor

    int divisor() const { return _divisor.get(isRouted(Routing::Target::Divisor)); }
    void setDivisor(int divisor, bool routed = false) {
        _divisor.set(ModelUtils::clampDivisor(divisor), routed);
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
        printRouted(str, Routing::Target::Divisor);
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

    Types::RunMode runMode() const { return _runMode.get(isRouted(Routing::Target::RunMode)); }
    void setRunMode(Types::RunMode runMode, bool routed = false) {
        _runMode.set(ModelUtils::clampedEnum(runMode), routed);
    }

    void editRunMode(int value, bool shift) {
        if (!isRouted(Routing::Target::RunMode)) {
            setRunMode(ModelUtils::adjustedEnum(runMode(), value));
        }
    }

    void printRunMode(StringBuilder &str) const {
        printRouted(str, Routing::Target::RunMode);
        str(Types::runModeName(runMode()));
    }

    // firstStep

    int firstStep() const { return _firstStep.get(isRouted(Routing::Target::FirstStep)); }
    void setFirstStep(int firstStep, bool routed = false) {
        _firstStep.set(clamp(firstStep, 0, lastStep()), routed);
    }

    void editFirstStep(int value, bool shift) {
        if (!isRouted(Routing::Target::FirstStep)) {
            setFirstStep(firstStep() + value);
        }
    }

    void printFirstStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::FirstStep);
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const { return _lastStep.get(isRouted(Routing::Target::LastStep)); }
    void setLastStep(int lastStep, bool routed = false) {
        _lastStep.set(clamp(lastStep, firstStep(), CONFIG_STEP_COUNT / 2 - 1), routed);
    }

    void editLastStep(int value, bool shift) {
        if (!isRouted(Routing::Target::LastStep)) {
            setLastStep(lastStep() + value);
        }
    }

    void printLastStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::LastStep);
        str("%d", lastStep() + 1);
    }

    // steps

    const StepArray &steps() const { return _steps; }
          StepArray &steps()       { return _steps; }

    const Step &step(int index) const { return _steps[index]; }
          Step &step(int index)       { return _steps[index]; }

    //----------------------------------------
    // Routing
    //----------------------------------------

    inline bool isRouted(Routing::Target target) const { return Routing::isRouted(target, _trackIndex); }
    inline void printRouted(StringBuilder &str, Routing::Target target) const { Routing::printRouted(str, target, _trackIndex); }
    void writeRouted(Routing::Target target, int intValue, float floatValue);

    //----------------------------------------
    // Methods
    //----------------------------------------

    StageSequence() { clear(); }

    void clear();
    void clearSteps();

    bool isEdited() const;

    void shiftSteps(int direction);

    void duplicateSteps();

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    int8_t _trackIndex = -1;
    int8_t _scale;
    int8_t _rootNote;
    Routable<uint16_t> _divisor;
    uint8_t _resetMeasure;
    Routable<Types::RunMode> _runMode;
    Routable<uint8_t> _firstStep;
    Routable<uint8_t> _lastStep;

    StepArray _steps;

    uint8_t _edited;

    friend class StageTrack;
};
