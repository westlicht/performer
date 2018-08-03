#include "NoteTrack.h"

void NoteTrack::clear() {
    _playMode = Types::PlayMode::Aligned;
    _fillMode = Types::FillMode::Gates;
    _slideTime = 50;
    _octave = 0;
    _transpose = 0;
    _rotate = 0;
    _stepProbabilityBias = 0;
    _stepLengthBias = 0;

    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void NoteTrack::write(WriteContext &context) const {
    auto writer = context.writer;
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_slideTime);
    writer.write(_octave);
    writer.write(_transpose);
    writer.write(_rotate);
    writer.write(_stepProbabilityBias);
    writer.write(_stepLengthBias);
    writeArray(context, _sequences);
}

void NoteTrack::read(ReadContext &context) {
    auto reader = context.reader;
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_slideTime);
    reader.read(_octave);
    reader.read(_transpose);
    reader.read(_rotate);
    reader.read(_stepProbabilityBias);
    reader.read(_stepLengthBias);
    readArray(context, _sequences);
}
