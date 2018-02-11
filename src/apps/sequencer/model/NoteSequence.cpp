#include "NoteSequence.h"

void NoteSequence::Step::write(WriteContext &context, int index) const {
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

void NoteSequence::Step::read(ReadContext &context, int index) {
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


void NoteSequence::write(WriteContext &context, int index) const {
    writeArray(context, _steps);
}

void NoteSequence::read(ReadContext &context, int index) {
    readArray(context, _steps);
}
