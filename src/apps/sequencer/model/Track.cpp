#include "Track.h"

void Track::write(WriteContext &context, int index) const {
    auto &writer = context.writer;
    writer.write(_mode);
}

void Track::read(ReadContext &context, int index) {
    auto &reader = context.reader;
    reader.read(_mode);
}
