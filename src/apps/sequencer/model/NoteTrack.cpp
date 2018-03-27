#include "NoteTrack.h"

void NoteTrack::clear() {
    _playMode = Types::PlayMode::Free;
    _fillMode = Types::FillMode::None;
    _transpose = 0;
    _rotate = 0;

    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void NoteTrack::write(WriteContext &context) const {
    auto writer = context.writer;
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_transpose);
    writer.write(_rotate);
    writeArray(context, _sequences);
}

void NoteTrack::read(ReadContext &context) {
    auto reader = context.reader;
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_transpose);
    reader.read(_rotate);
    readArray(context, _sequences);
}
