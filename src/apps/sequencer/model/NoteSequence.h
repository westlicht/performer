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
#include <bitset>
#include <cstdint>
#include <initializer_list>

class NoteSequence {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    typedef UnsignedValue<3> GateProbability;
    typedef SignedValue<4> GateOffset;
    typedef UnsignedValue<2> Retrigger;
    typedef UnsignedValue<3> RetriggerProbability;
    typedef UnsignedValue<3> Length;
    typedef SignedValue<4> LengthVariationRange;
    typedef UnsignedValue<3> LengthVariationProbability;
    typedef SignedValue<7> Note;
    typedef SignedValue<7> NoteVariationRange;
    typedef UnsignedValue<3> NoteVariationProbability;
    typedef UnsignedValue<7> Condition;
    typedef UnsignedValue<3> StageRepeats;
    typedef UnsignedValue<2> StageRepeatsMode;

    static_assert(int(Types::Condition::Last) <= Condition::Max + 1, "Condition enum does not fit");

    enum class Layer {
        Gate,
        GateProbability,
        GateOffset,
        Slide,
        Retrigger,
        RetriggerProbability,
        Length,
        LengthVariationRange,
        LengthVariationProbability,
        Note,
        NoteVariationRange,
        NoteVariationProbability,
        Condition,
        StageRepeats,
        StageRepeatsMode,
        Last
    };

    static const char *layerName(Layer layer) {
        switch (layer) {
        case Layer::Gate:                       return "GATE";
        case Layer::GateProbability:            return "GATE PROB";
        case Layer::GateOffset:                 return "GATE OFFSET";
        case Layer::Slide:                      return "SLIDE";
        case Layer::Retrigger:                  return "RETRIG";
        case Layer::RetriggerProbability:       return "RETRIG PROB";
        case Layer::Length:                     return "LENGTH";
        case Layer::LengthVariationRange:       return "LENGTH RANGE";
        case Layer::LengthVariationProbability: return "LENGTH PROB";
        case Layer::Note:                       return "NOTE";
        case Layer::NoteVariationRange:         return "NOTE RANGE";
        case Layer::NoteVariationProbability:   return "NOTE PROB";
        case Layer::Condition:                  return "CONDITION";
        case Layer::StageRepeats:               return "REPEAT";
        case Layer::StageRepeatsMode:           return "REPEAT MODE";
        case Layer::Last:                       break;
        }
        return nullptr;
    }

    static Types::LayerRange layerRange(Layer layer);
    static int layerDefaultValue(Layer layer);

    enum StageRepeatMode {
        Each,
        First,
        Odd,
        Triplets
    };

    class Step {

    public:
        //----------------------------------------
        // Properties
        //----------------------------------------
        
        // stage
        void setStageRepeats(int repeats) {
            _data1.stageRepeats = StageRepeats::clamp(repeats - 1);
        }
        unsigned int stageRepeats() const { return _data1.stageRepeats + 1; }

        void setStageRepeatsMode(StageRepeatMode mode) {
            _data1.stageRepeatMode = mode;
        }

        StageRepeatMode stageRepeatMode() const { 
            int value = _data1.stageRepeatMode;
            return static_cast<StageRepeatMode>(value); 
        }

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

        // condition

        Types::Condition condition() const { return Types::Condition(int(_data1.condition)); }
        void setCondition(Types::Condition condition) {
            _data1.condition = int(ModelUtils::clampedEnum(condition));
        }

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
            return _data0.raw == other._data0.raw && _data1.raw == other._data1.raw;
        }

        bool operator!=(const Step &other) const {
            return !(*this == other);
        }

    private:
        union {
            uint32_t raw;
            BitField<uint32_t, 0, 1> gate;
            BitField<uint32_t, 1, 1> slide;
            BitField<uint32_t, 2, GateProbability::Bits> gateProbability;
            BitField<uint32_t, 5, Length::Bits> length;
            BitField<uint32_t, 8, LengthVariationRange::Bits> lengthVariationRange;
            BitField<uint32_t, 12, LengthVariationProbability::Bits> lengthVariationProbability;
            BitField<uint32_t, 15, Note::Bits> note;
            BitField<uint32_t, 22, NoteVariationRange::Bits> noteVariationRange;
            BitField<uint32_t, 29, NoteVariationProbability::Bits> noteVariationProbability;
        } _data0;
        union {
            uint32_t raw;
            BitField<uint32_t, 0, Retrigger::Bits> retrigger;
            BitField<uint32_t, 2, RetriggerProbability::Bits> retriggerProbability;
            BitField<uint32_t, 5, GateOffset::Bits> gateOffset;
            BitField<uint32_t, 9, Condition::Bits> condition;
            BitField<uint32_t, 16, StageRepeats::Bits> stageRepeats;
            BitField<uint32_t, 19, StageRepeatsMode::Bits> stageRepeatMode;
            // 12 bits left
        } _data1;
    };

    typedef std::array<Step, CONFIG_STEP_COUNT> StepArray;

    //----------------------------------------
    // Properties
    //----------------------------------------

    // trackIndex

    int trackIndex() const { return _trackIndex; }

    // scale

    int scale() const { return _scale.get(isRouted(Routing::Target::Scale)); }
    void setScale(int scale, bool routed = false) {
        _scale.set(clamp(scale, -1, Scale::Count - 1), routed);
    }

    int indexedScale() const { return scale() + 1; }
    void setIndexedScale(int index) {
        setScale(index - 1);
    }

    void editScale(int value, bool shift) {
        if (!isRouted(Routing::Target::Scale)) {
            setScale(scale() + value);
        }
    }

    void printScale(StringBuilder &str) const {
        printRouted(str, Routing::Target::Scale);
        str(scale() < 0 ? "Default" : Scale::name(scale()));
    }

    const Scale &selectedScale(int defaultScale) const {
        return Scale::get(scale() < 0 ? defaultScale : scale());
    }

    // rootNote

    int rootNote() const { return _rootNote.get(isRouted(Routing::Target::RootNote)); }
    void setRootNote(int rootNote, bool routed = false) {
        _rootNote.set(clamp(rootNote, -1, 11), routed);
    }

    int indexedRootNote() const { return rootNote() + 1; }
    void setIndexedRootNote(int index) {
        setRootNote(index - 1);
    }

    void editRootNote(int value, bool shift) {
        if (!isRouted(Routing::Target::RootNote)) {
            setRootNote(rootNote() + value);
        }
    }

    void printRootNote(StringBuilder &str) const {
        printRouted(str, Routing::Target::RootNote);
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
        if (!isRouted(Routing::Target::Divisor)) {
            setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
        }
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
            str("%d %s", resetMeasure(), resetMeasure() > 1 ? "bars" : "bar");
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

    int firstStep() const {
        return _firstStep.get(isRouted(Routing::Target::FirstStep));
    }

    void setFirstStep(int firstStep, bool routed = false) {
        _firstStep.set(clamp(firstStep, 0, lastStep()), routed);
    }

    void editFirstStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::FirstStep)) {
            setFirstStep(firstStep() + value);
        }
    }

    void printFirstStep(StringBuilder &str) const {
        printRouted(str, Routing::Target::FirstStep);
        str("%d", firstStep() + 1);
    }

    // lastStep

    int lastStep() const {
        // make sure last step is always >= first step even if stored value is invalid (due to routing changes)
        return std::max(firstStep(), int(_lastStep.get(isRouted(Routing::Target::LastStep))));
    }

    void setLastStep(int lastStep, bool routed = false) {
        _lastStep.set(clamp(lastStep, firstStep(), CONFIG_STEP_COUNT - 1), routed);
    }

    void editLastStep(int value, bool shift) {
        if (shift) {
            offsetFirstAndLastStep(value);
        } else if (!isRouted(Routing::Target::LastStep)) {
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

    NoteSequence() { clear(); }

    void clear();
    void clearSteps();

    bool isEdited() const;

    void setGates(std::initializer_list<int> gates);
    void setNotes(std::initializer_list<int> notes);

    void shiftSteps(const std::bitset<CONFIG_STEP_COUNT> &selected, int direction);

    void duplicateSteps();

    void write(VersionedSerializedWriter &writer) const;
    void read(VersionedSerializedReader &reader);

private:
    void setTrackIndex(int trackIndex) { _trackIndex = trackIndex; }

    void offsetFirstAndLastStep(int value) {
        value = clamp(value, -firstStep(), CONFIG_STEP_COUNT - 1 - lastStep());
        if (value > 0) {
            editLastStep(value, false);
            editFirstStep(value, false);
        } else {
            editFirstStep(value, false);
            editLastStep(value, false);
        }
    }

    int8_t _trackIndex = -1;
    Routable<int8_t> _scale;
    Routable<int8_t> _rootNote;
    Routable<uint16_t> _divisor;
    uint8_t _resetMeasure;
    Routable<Types::RunMode> _runMode;
    Routable<uint8_t> _firstStep;
    Routable<uint8_t> _lastStep;

    StepArray _steps;

    uint8_t _edited;

    friend class NoteTrack;
};
