#include "NoteSequence.h"

#include "ModelUtils.h"

uint8_t NoteSequence::_defaultScale;
uint8_t NoteSequence::_defaultRootNote;

Types::LayerRange NoteSequence::layerRange(Layer layer) {
    #define CASE(_layer_) \
    case Layer::_layer_: \
        return { _layer_::Min, _layer_::Max };

    switch (layer) {
    case Layer::Gate: return { 0, 1 };
    case Layer::Slide: return { 0, 1 };
    CASE(GateProbability)
    CASE(Retrigger)
    CASE(RetriggerProbability)
    CASE(Length)
    CASE(LengthVariationRange)
    CASE(LengthVariationProbability)
    CASE(Note)
    CASE(NoteVariationRange)
    CASE(NoteVariationProbability)
    case Layer::Last: break;
    }

    #undef CASE

    return { 0, 0 };
}

int NoteSequence::Step::layerValue(Layer layer) const {
    #define CASE(_layer_, _data_) \
    case Layer::_layer_: \
        return _data_ + _layer_::Min;

    switch (layer) {
    case Layer::Gate: return _data0.gate ? 1 : 0;
    case Layer::Slide: return _data0.slide ? 1 : 0;
    CASE(GateProbability, _data0.gateProbability)
    CASE(Retrigger, _data1.retrigger)
    CASE(RetriggerProbability, _data1.retriggerProbability)
    CASE(Length, _data0.length)
    CASE(LengthVariationRange, _data0.lengthVariationRange)
    CASE(LengthVariationProbability, _data0.lengthVariationProbability)
    CASE(Note, _data0.note)
    CASE(NoteVariationRange, _data0.noteVariationRange)
    CASE(NoteVariationProbability, _data0.noteVariationProbability)
    case Layer::Last: break;
    }

    #undef CASE

    return 0;
}

void NoteSequence::Step::setLayerValue(Layer layer, int value) {
    #define CASE(_layer_, _data_) \
    case Layer::_layer_: \
        _data_ = value - _layer_::Min; \
        break;

    switch (layer) {
    case Layer::Gate: _data0.gate = value; break;
    case Layer::Slide: _data0.slide = value; break;
    CASE(GateProbability, _data0.gateProbability)
    CASE(Retrigger, _data1.retrigger)
    CASE(RetriggerProbability, _data1.retriggerProbability)
    CASE(Length, _data0.length)
    CASE(LengthVariationRange, _data0.lengthVariationRange)
    CASE(LengthVariationProbability, _data0.lengthVariationProbability)
    CASE(Note, _data0.note)
    CASE(NoteVariationRange, _data0.noteVariationRange)
    CASE(NoteVariationProbability, _data0.noteVariationProbability)
    case Layer::Last: break;
    }

    #undef CASE
}

void NoteSequence::Step::clear() {
    setGate(false);
    setSlide(false);
    setRetrigger(0);
    setRetriggerProbability(RetriggerProbability::Max);
    setGateProbability(GateProbability::Max);
    setLength(Length::Max / 2);
    setLengthVariationRange(0);
    setLengthVariationProbability(0);
    setNote(0);
    setNoteVariationRange(0);
    setNoteVariationProbability(NoteVariationProbability::Max);
}

void NoteSequence::Step::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_data0.raw);
    writer.write(_data1.raw);

#if 0
    bool gateValue = gate();
    writer.write(gateValue);

    uint8_t gateProbabilityValue = gateProbability();
    writer.write(gateProbabilityValue);

    uint8_t lengthValue = length();
    writer.write(lengthValue);

    uint8_t lengthVariationRangeValue = lengthVariationRange();
    writer.write(lengthVariationRangeValue);

    uint8_t lengthVariationProbabilityValue = lengthVariationProbability();
    writer.write(lengthVariationProbabilityValue);

    uint8_t noteValue = note();
    writer.write(noteValue);

    uint8_t noteVariationRangeValue = noteVariationRange();
    writer.write(noteVariationRangeValue);

    uint8_t noteVariationProbabilityValue = noteVariationProbability();
    writer.write(noteVariationProbabilityValue);
#endif
}

void NoteSequence::Step::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_data0.raw);
    reader.read(_data1.raw);

#if 0
    bool gateValue;
    reader.read(gateValue);
    setGate(gateValue);

    uint8_t gateProbabilityValue;
    reader.read(gateProbabilityValue);
    setGateProbability(gateProbabilityValue);

    uint8_t lengthValue;
    reader.read(lengthValue);
    setLength(lengthValue);

    uint8_t lengthVariationRangeValue;
    reader.read(lengthVariationRangeValue);
    setLengthVariationRange(lengthVariationRangeValue);

    uint8_t lengthVariationProbabilityValue;
    reader.read(lengthVariationProbabilityValue);
    setLengthVariationProbability(lengthVariationProbabilityValue);

    uint8_t noteValue;
    reader.read(noteValue);
    setNote(noteValue);

    uint8_t noteVariationRangeValue;
    reader.read(noteVariationRangeValue);
    setNoteVariationRange(noteVariationRangeValue);

    uint8_t noteVariationProbabilityValue;
    reader.read(noteVariationProbabilityValue);
    setNoteVariationProbability(noteVariationProbabilityValue);
#endif
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

void NoteSequence::shiftSteps(int direction) {
    ModelUtils::shiftSteps(_steps, direction);
}

void NoteSequence::duplicateSteps() {
    ModelUtils::duplicateSteps(_steps, firstStep(), lastStep());
    setLastStep(lastStep() + (lastStep() - firstStep() + 1));
}

void NoteSequence::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_scale);
    writer.write(_rootNote);
    writer.write(_divisor);
    writer.write(_resetMeasure);
    writer.write(_runMode);
    writer.write(_firstStep);
    writer.write(_lastStep);

    writeArray(context, _steps);
}

void NoteSequence::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_scale);
    reader.read(_rootNote);
    reader.read(_divisor);
    reader.read(_resetMeasure);
    reader.read(_runMode);
    reader.read(_firstStep);
    reader.read(_lastStep);

    readArray(context, _steps);
}
