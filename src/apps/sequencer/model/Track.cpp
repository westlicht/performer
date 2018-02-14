#include "Track.h"

void Track::write(WriteContext &context, int index) const {
    auto &writer = context.writer;
    writer.write(_mode);
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_linkTrack);
}

void Track::read(ReadContext &context, int index) {
    auto &reader = context.reader;
    reader.read(_mode);
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_linkTrack);
}
