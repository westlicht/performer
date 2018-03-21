#include "Track.h"

void Track::clear() {
    _trackMode = Types::TrackMode::Default;
    _playMode = PlayMode::Free;
    _fillMode = FillMode::None;
    _linkTrack = -1;
}

void Track::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_trackMode);
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_linkTrack);
}

void Track::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_trackMode);
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_linkTrack);
}
