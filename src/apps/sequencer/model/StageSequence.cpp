#include "StageSequence.h"
#include "Project.h"

#include "ModelUtils.h"

Types::LayerRange StageSequence::layerRange(Layer layer) {
    #define CASE(_layer_) \
    case Layer::_layer_: \
        return { _layer_::Min, _layer_::Max };
    #define CASE2(_layer_) \
    case Layer::_layer_##0: \
    case Layer::_layer_##1: \
        return { _layer_::Min, _layer_::Max };

    switch (layer) {
    CASE(Length)
    // case Layer::Gate:
    //     return { 0, 1 };
    // case Layer::Slide:
    //     return { 0, 1 };
    // case Layer::GateOffset:
    //     // TODO: allow negative gate delay in the future
    //     return { 0, GateOffset::Max };
    // CASE(GateProbability)
    // CASE(Retrigger)
    // CASE(RetriggerProbability)
    // CASE(Length)
    // CASE(LengthVariationRange)
    // CASE(LengthVariationProbability)
    CASE2(Note)
    CASE2(GateCount)
    // CASE(NoteVariationRange)
    // CASE(NoteVariationProbability)
    case Layer::Last:
        break;
    }

    #undef CASE
    #undef CASE2

    return { 0, 0 };
}

int StageSequence::Step::layerValue(Layer layer) const {
    switch (layer) {
    case Layer::Length:
        return length();
    // case Layer::Gate:
    //     return gate() ? 1 : 0;
    // case Layer::Slide:
    //     return slide() ? 1 : 0;
    // case Layer::GateProbability:
    //     return gateProbability();
    // case Layer::GateOffset:
    //     return gateOffset();
    // case Layer::Retrigger:
    //     return retrigger();
    // case Layer::RetriggerProbability:
    //     return retriggerProbability();
    // case Layer::Length:
    //     return length();
    // case Layer::LengthVariationRange:
    //     return lengthVariationRange();
    // case Layer::LengthVariationProbability:
    //     return lengthVariationProbability();
    case Layer::Note0:
        return note(0);
    case Layer::Note1:
        return note(1);
    case Layer::GateCount0:
        return gateCount(0);
    case Layer::GateCount1:
        return gateCount(1);
    // case Layer::NoteVariationRange:
    //     return noteVariationRange();
    // case Layer::NoteVariationProbability:
    //     return noteVariationProbability();
    case Layer::Last:
        break;
    }

    return 0;
}

void StageSequence::Step::setLayerValue(Layer layer, int value) {
    switch (layer) {
    case Layer::Length:
        setLength(value);
    // case Layer::Gate:
    //     setGate(value);
    //     break;
    // case Layer::Slide:
    //     setSlide(value);
    //     break;
    // case Layer::GateProbability:
    //     setGateProbability(value);
    //     break;
    // case Layer::GateOffset:
    //     setGateOffset(value);
    //     break;
    // case Layer::Retrigger:
    //     setRetrigger(value);
    //     break;
    // case Layer::RetriggerProbability:
    //     setRetriggerProbability(value);
    //     break;
    // case Layer::Length:
    //     setLength(value);
    //     break;
    // case Layer::LengthVariationRange:
    //     setLengthVariationRange(value);
    //     break;
    // case Layer::LengthVariationProbability:
    //     setLengthVariationProbability(value);
    //     break;
    case Layer::Note0:
        setNote(0, value);
        break;
    case Layer::Note1:
        setNote(1, value);
        break;
    case Layer::GateCount0:
        setGateCount(0, value);
        break;
    case Layer::GateCount1:
        setGateCount(1, value);
        break;
    // case Layer::NoteVariationRange:
    //     setNoteVariationRange(value);
    //     break;
    // case Layer::NoteVariationProbability:
    //     setNoteVariationProbability(value);
    //     break;
    case Layer::Last:
        break;
    }
}

void StageSequence::Step::clear() {
    _data0.raw = 0;
    _dataChannel[0].raw = 0;
    _dataChannel[1].raw = 0;
    setLength(0);
    for (int layer = 0; layer < 2; ++layer) {
        setNote(layer, 0);
        setGateCount(layer, 0);
    }
    // setGate(false);
    // setGateProbability(GateProbability::Max);
    // setGateOffset(0);
    // setSlide(false);
    // setRetrigger(0);
    // setRetriggerProbability(RetriggerProbability::Max);
    // setLength(Length::Max / 2);
    // setLengthVariationRange(0);
    // setLengthVariationProbability(LengthVariationProbability::Max);
    // setNote(0);
    // setNoteVariationRange(0);
    // setNoteVariationProbability(NoteVariationProbability::Max);
}

void StageSequence::Step::write(VersionedSerializedWriter &writer) const {
    writer.write(_data0.raw);
    writer.write(_dataChannel[0].raw);
    writer.write(_dataChannel[1].raw);
}

void StageSequence::Step::read(VersionedSerializedReader &reader) {
    reader.read(_data0.raw);
    reader.read(_dataChannel[0].raw);
    reader.read(_dataChannel[1].raw);
}

void StageSequence::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
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

void StageSequence::clear() {
    setScale(-1);
    setRootNote(-1);
    setDivisor(12);
    setResetMeasure(0);
    setRunMode(Types::RunMode::Forward);
    setFirstStep(0);
    setLastStep(15);

    clearSteps();
}

void StageSequence::clearSteps() {
    for (auto &step : _steps) {
        step.clear();
    }
}

bool StageSequence::isEdited() const {
    auto clearStep = Step();
    for (const auto &step : _steps) {
        if (step != clearStep) {
            return true;
        }
    }
    return false;
}

void StageSequence::shiftSteps(int direction) {
    ModelUtils::shiftSteps(_steps, direction);
}

void StageSequence::duplicateSteps() {
    ModelUtils::duplicateSteps(_steps, firstStep(), lastStep());
    setLastStep(lastStep() + (lastStep() - firstStep() + 1));
}

void StageSequence::write(VersionedSerializedWriter &writer) const {
    writer.write(_scale);
    writer.write(_rootNote);
    writer.write(_divisor.base);
    writer.write(_resetMeasure);
    writer.write(_runMode.base);
    writer.write(_firstStep.base);
    writer.write(_lastStep.base);

    writeArray(writer, _steps);
}

void StageSequence::read(VersionedSerializedReader &reader) {
    reader.read(_scale);
    reader.read(_rootNote);
    reader.read(_divisor.base);
    reader.read(_resetMeasure);
    reader.read(_runMode.base);
    reader.read(_firstStep.base);
    reader.read(_lastStep.base);

    readArray(reader, _steps);
}
