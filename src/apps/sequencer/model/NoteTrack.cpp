#include "NoteTrack.h"
#include "Project.h"

void NoteTrack::clear() {
    _playMode = Types::PlayMode::Aligned;
    _fillMode = Types::FillMode::Gates;
    _cvUpdateMode = CvUpdateMode::Gate;
    _slideTime = 50;
    _octave = 0;
    _transpose = 0;
    _rotate = 0;
    _gateProbabilityBias = 0;
    _retriggerProbabilityBias = 0;
    _lengthBias = 0;
    _noteProbabilityBias = 0;

    for (auto &sequence : _sequences) {
        sequence.clear();
    }
}

void NoteTrack::write(WriteContext &context) const {
    auto writer = context.writer;
    writer.write(_playMode);
    writer.write(_fillMode);
    writer.write(_cvUpdateMode);
    writer.write(_slideTime);
    writer.write(_octave);
    writer.write(_transpose);
    writer.write(_rotate);
    writer.write(_gateProbabilityBias);
    writer.write(_retriggerProbabilityBias);
    writer.write(_lengthBias);
    writer.write(_noteProbabilityBias);
    writeArray(context, _sequences);
}

void NoteTrack::read(ReadContext &context) {
    auto reader = context.reader;
    reader.read(_playMode);
    reader.read(_fillMode);
    reader.read(_cvUpdateMode, Project::Version4);
    reader.read(_slideTime);
    reader.read(_octave);
    reader.read(_transpose);
    reader.read(_rotate);
    reader.read(_gateProbabilityBias);
    reader.read(_retriggerProbabilityBias);
    reader.read(_lengthBias);
    reader.read(_noteProbabilityBias);
    readArray(context, _sequences);
}
