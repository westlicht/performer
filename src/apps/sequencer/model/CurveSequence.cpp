#include "CurveSequence.h"

#include "ModelUtils.h"

void CurveSequence::Step::clear() {
    setShape(0);
    setMin(0);
    setMax(Max::Max);
}

void CurveSequence::Step::write(WriteContext &context, int index) const {
    auto &writer = context.writer;
    writer.write(_shape);
    writer.write(_min);
    writer.write(_max);
}

void CurveSequence::Step::read(ReadContext &context, int index) {
    auto &reader = context.reader;
    reader.read(_shape);
    reader.read(_min);
    reader.read(_max);
}

void CurveSequence::clear() {
    setRange(Range::Bipolar5V);
    setDivisor(4);
    setResetMeasure(0);
    setPlayMode(PlayMode::Forward);
    setFirstStep(0);
    setLastStep(CONFIG_STEP_COUNT - 1);
    for (auto &step : _steps) {
        step.clear();
    }
}

void CurveSequence::setShapes(std::initializer_list<int> shapes) {
    size_t step = 0;
    for (auto shape : shapes) {
        if (step < _steps.size()) {
            _steps[step++].setShape(shape);
        }
    }
}

void CurveSequence::shift(int direction) {
    ModelUtils::shiftSteps(_steps, direction);
}

void CurveSequence::write(WriteContext &context, int index) const {
    auto &writer = context.writer;
    writer.write(_range);
    writer.write(_divisor);
    writer.write(_resetMeasure);
    writer.write(_playMode);
    writer.write(_firstStep);
    writer.write(_lastStep);

    writeArray(context, _steps);
}

void CurveSequence::read(ReadContext &context, int index) {
    auto &reader = context.reader;
    reader.read(_range);
    reader.read(_divisor);
    reader.read(_resetMeasure);
    reader.read(_playMode);
    reader.read(_firstStep);
    reader.read(_lastStep);

    readArray(context, _steps);
}
