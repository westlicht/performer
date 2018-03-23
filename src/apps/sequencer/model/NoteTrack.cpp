#include "NoteTrack.h"

void NoteTrack::clear() {
    _playMode = Types::PlayMode::Free;
    _fillMode = Types::FillMode::None;
    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void NoteTrack::write(WriteContext &context) const {
    auto writer = context.writer;
    writer.write(_playMode);
    writer.write(_fillMode);
    writeArray(context, _sequences);
}

void NoteTrack::read(ReadContext &context) {
    auto reader = context.reader;
    reader.read(_playMode);
    reader.read(_fillMode);
    readArray(context, _sequences);
}
