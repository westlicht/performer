#include "CurveSequence.h"

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

void CurveSequence::write(WriteContext &context, int index) const {
    writeArray(context, _steps);
}

void CurveSequence::read(ReadContext &context, int index) {
    readArray(context, _steps);
}
