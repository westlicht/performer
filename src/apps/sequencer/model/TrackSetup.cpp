#include "TrackSetup.h"

void TrackSetup::clear() {
    _trackMode = Types::TrackMode::Default;
    _playMode = PlayMode::Free;
    _fillMode = FillMode::None;
    _linkTrack = -1;
}

void TrackSetup::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_trackMode);
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_linkTrack);
}

void TrackSetup::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_trackMode);
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_linkTrack);
}
