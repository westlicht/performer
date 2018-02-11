#include "CurveSequence.h"

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


void CurveSequence::write(WriteContext &context, int index) const {
    writeArray(context, _steps);
}

void CurveSequence::read(ReadContext &context, int index) {
    readArray(context, _steps);
}
