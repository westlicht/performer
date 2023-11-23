#include "NoteSequence.h"
#include "ProjectVersion.h"

#include "ModelUtils.h"

Types::LayerRange NoteSequence::layerRange(Layer layer) {
    #define CASE(_layer_) \
    case Layer::_layer_: \
        return { _layer_::Min, _layer_::Max };

    switch (layer) {
    case Layer::Gate:
        return { 0, 1 };
    case Layer::Slide:
        return { 0, 1 };
    CASE(GateOffset)
    CASE(GateProbability)
    CASE(Retrigger)
    CASE(RetriggerProbability)
    CASE(Length)
    CASE(LengthVariationRange)
    CASE(LengthVariationProbability)
    CASE(Note)
    CASE(NoteVariationRange)
    CASE(NoteVariationProbability)
    CASE(Condition)
    case Layer::Last:
        break;
    }

    #undef CASE

    return { 0, 0 };
}

int NoteSequence::layerDefaultValue(Layer layer)
{
    NoteSequence::Step step;

    switch (layer) {
    case Layer::Gate:
        return step.gate();
    case Layer::GateProbability:
        return step.gateProbability();
    case Layer::GateOffset:
        return step.gateOffset();
    case Layer::Slide:
        return step.slide();
    case Layer::Retrigger:
        return step.retrigger();
    case Layer::RetriggerProbability:
        return step.retriggerProbability();
    case Layer::Length:
        return step.length();
    case Layer::LengthVariationRange:
        return step.lengthVariationRange();
    case Layer::LengthVariationProbability:
        return step.lengthVariationProbability();
    case Layer::Note:
        return step.note();
    case Layer::NoteVariationRange:
        return step.noteVariationRange();
    case Layer::NoteVariationProbability:
        return step.noteVariationProbability();
    case Layer::Condition:
        return int(step.condition());
    case Layer::Last:
        break;
    }

    return 0;
}

int NoteSequence::Step::layerValue(Layer layer) const {
    switch (layer) {
    case Layer::Gate:
        return gate() ? 1 : 0;
    case Layer::Slide:
        return slide() ? 1 : 0;
    case Layer::GateProbability:
        return gateProbability();
    case Layer::GateOffset:
        return gateOffset();
    case Layer::Retrigger:
        return retrigger();
    case Layer::RetriggerProbability:
        return retriggerProbability();
    case Layer::Length:
        return length();
    case Layer::LengthVariationRange:
        return lengthVariationRange();
    case Layer::LengthVariationProbability:
        return lengthVariationProbability();
    case Layer::Note:
        return note();
    case Layer::NoteVariationRange:
        return noteVariationRange();
    case Layer::NoteVariationProbability:
        return noteVariationProbability();
    case Layer::Condition:
        return int(condition());
    case Layer::Last:
        break;
    }

    return 0;
}

void NoteSequence::Step::setLayerValue(Layer layer, int value) {
    switch (layer) {
    case Layer::Gate:
        setGate(value);
        break;
    case Layer::Slide:
        setSlide(value);
        break;
    case Layer::GateProbability:
        setGateProbability(value);
        break;
    case Layer::GateOffset:
        setGateOffset(value);
        break;
    case Layer::Retrigger:
        setRetrigger(value);
        break;
    case Layer::RetriggerProbability:
        setRetriggerProbability(value);
        break;
    case Layer::Length:
        setLength(value);
        break;
    case Layer::LengthVariationRange:
        setLengthVariationRange(value);
        break;
    case Layer::LengthVariationProbability:
        setLengthVariationProbability(value);
        break;
    case Layer::Note:
        setNote(value);
        break;
    case Layer::NoteVariationRange:
        setNoteVariationRange(value);
        break;
    case Layer::NoteVariationProbability:
        setNoteVariationProbability(value);
        break;
    case Layer::Condition:
        setCondition(Types::Condition(value));
        break;
    case Layer::Last:
        break;
    }
}

void NoteSequence::Step::clear() {
    _data0.raw = 0;
    _data1.raw = 1;
    setGate(false);
    setGateProbability(GateProbability::Max);
    setGateOffset(0);
    setSlide(false);
    setRetrigger(0);
    setRetriggerProbability(RetriggerProbability::Max);
    setLength(Length::Max / 2);
    setLengthVariationRange(0);
    setLengthVariationProbability(LengthVariationProbability::Max);
    setNote(0);
    setNoteVariationRange(0);
    setNoteVariationProbability(NoteVariationProbability::Max);
    setCondition(Types::Condition::Off);
}

void NoteSequence::Step::write(VersionedSerializedWriter &writer) const {
    writer.write(_data0.raw);
    writer.write(_data1.raw);
}

void NoteSequence::Step::read(VersionedSerializedReader &reader) {
    if (reader.dataVersion() < ProjectVersion::Version27) {
        reader.read(_data0.raw);
        reader.readAs<uint16_t>(_data1.raw);
        if (reader.dataVersion() < ProjectVersion::Version5) {
            _data1.raw &= 0x1f;
        }
        if (reader.dataVersion() < ProjectVersion::Version7) {
            setGateOffset(0);
        }
        if (reader.dataVersion() < ProjectVersion::Version12) {
            setCondition(Types::Condition(0));
        }
    } else {
        reader.read(_data0.raw);
        reader.read(_data1.raw);
    }
}

void NoteSequence::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::Scale:
        setScale(intValue, true);
        break;
    case Routing::Target::RootNote:
        setRootNote(intValue, true);
        break;
    case Routing::Target::Divisor:
        setDivisor(intValue, true);
        break;
    case Routing::Target::RunMode:
        setRunMode(Types::RunMode(intValue), true);
        break;
    case Routing::Target::FirstStep:
        setFirstStep(intValue, true);
        break;
    case Routing::Target::LastStep:
        setLastStep(intValue, true);
        break;
    default:
        break;
    }
}

void NoteSequence::clear() {
    setScale(-1);
    setRootNote(-1);
    setDivisor(12);
    setResetMeasure(0);
    setRunMode(Types::RunMode::Forward);
    setFirstStep(0);
    setLastStep(15);

    clearSteps();
}

void NoteSequence::clearSteps() {
    for (auto &step : _steps) {
        step.clear();
    }
}

bool NoteSequence::isEdited() const {
    auto clearStep = Step();
    for (const auto &step : _steps) {
        if (step != clearStep) {
            return true;
        }
    }
    return false;
}

void NoteSequence::setGates(std::initializer_list<int> gates) {
    size_t step = 0;
    for (auto gate : gates) {
        if (step < _steps.size()) {
            _steps[step++].setGate(gate);
        }
    }
}

void NoteSequence::setNotes(std::initializer_list<int> notes) {
    size_t step = 0;
    for (auto note : notes) {
        if (step < _steps.size()) {
            _steps[step++].setNote(note);
        }
    }
}

void NoteSequence::shiftSteps(const std::bitset<CONFIG_STEP_COUNT> &selected, int direction) {
    if (selected.any()) {
        ModelUtils::shiftSteps(_steps, selected, direction);
    } else {
        ModelUtils::shiftSteps(_steps, firstStep(), lastStep(), direction);
    }
}

void NoteSequence::duplicateSteps() {
    ModelUtils::duplicateSteps(_steps, firstStep(), lastStep());
    setLastStep(lastStep() + (lastStep() - firstStep() + 1));
}

void NoteSequence::write(VersionedSerializedWriter &writer) const {
    writer.write(_scale.base);
    writer.write(_rootNote.base);
    writer.write(_divisor.base);
    writer.write(_resetMeasure);
    writer.write(_runMode.base);
    writer.write(_firstStep.base);
    writer.write(_lastStep.base);

    writeArray(writer, _steps);
}

void NoteSequence::read(VersionedSerializedReader &reader) {
    reader.read(_scale.base);
    reader.read(_rootNote.base);
    if (reader.dataVersion() < ProjectVersion::Version10) {
        reader.readAs<uint8_t>(_divisor.base);
    } else {
        reader.read(_divisor.base);
    }
    reader.read(_resetMeasure);
    reader.read(_runMode.base);
    reader.read(_firstStep.base);
    reader.read(_lastStep.base);

    readArray(reader, _steps);
}
