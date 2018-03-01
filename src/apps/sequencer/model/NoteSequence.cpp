#include "NoteSequence.h"

#include "ModelUtils.h"

void NoteSequence::Step::clear() {
    setGate(false);
    setGateProbability(GateProbability::Max);
    setLength(Length::Max / 2);
    setLengthVariationRange(0);
    setLengthVariationProbability(0);
    setNote(0);
}

void NoteSequence::Step::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_data0.raw);

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
    setScale(3);
    setDivisor(4);
    setResetMeasure(0);
    setPlayMode(PlayMode::Forward);
    setFirstStep(0);
    setLastStep(CONFIG_STEP_COUNT - 1);
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

void NoteSequence::shift(int direction) {
    ModelUtils::shiftSteps(_steps, direction);
}

void NoteSequence::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_scale);
    writer.write(_divisor);
    writer.write(_resetMeasure);
    writer.write(_playMode);
    writer.write(_firstStep);
    writer.write(_lastStep);

    writeArray(context, _steps);
}

void NoteSequence::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_scale);
    reader.read(_divisor);
    reader.read(_resetMeasure);
    reader.read(_playMode);
    reader.read(_firstStep);
    reader.read(_lastStep);

    readArray(context, _steps);
}
