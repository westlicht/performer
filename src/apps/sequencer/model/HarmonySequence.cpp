#include "HarmonySequence.h"

#include "ModelUtils.h"

void HarmonySequence::Step::clear() {
    setLength(4);
    setChordRoot(0);
    setChordQuality(Chord::Quality(0));
    setChordVoicing(Chord::Voicing(0));
}

void HarmonySequence::Step::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_data0.raw);
    writer.write(_data1.raw);
}

void HarmonySequence::Step::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_data0.raw);
    reader.read(_data1.raw);
}

void HarmonySequence::clear() {
    setDivisor(12);
    setResetMeasure(0);
    setRunMode(Types::RunMode::Forward);
    setFirstStep(0);
    setLastStep(15);
    clearSteps();
}

void HarmonySequence::clearSteps() {
    for (auto &step : _steps) {
        step.clear();
    }
}

void HarmonySequence::shiftSteps(int direction) {
    ModelUtils::shiftSteps(_steps, direction);
}

void HarmonySequence::duplicateSteps() {
    ModelUtils::duplicateSteps(_steps, firstStep(), lastStep());
    setLastStep(lastStep() + (lastStep() - firstStep() + 1));
}

void HarmonySequence::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_divisor);
    writer.write(_resetMeasure);
    writer.write(_runMode);
    writer.write(_firstStep);
    writer.write(_lastStep);

    writeArray(context, _steps);
}

void HarmonySequence::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_divisor);
    reader.read(_resetMeasure);
    reader.read(_runMode);
    reader.read(_firstStep);
    reader.read(_lastStep);

    readArray(context, _steps);
}
