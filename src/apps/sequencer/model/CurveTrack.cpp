#include "CurveTrack.h"

void CurveTrack::clear() {
    _playMode = Types::PlayMode::Aligned;
    _fillMode = Types::FillMode::None;
    _rotate = 0;

    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void CurveTrack::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_rotate);
    writeArray(context, _sequences);
}

void CurveTrack::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_rotate);
    readArray(context, _sequences);
}
