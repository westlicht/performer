#include "Project.h"

void Project::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_bpm);
    writer.write(_swing);

    _clockSetup.write(context);

    writeArray(context, _tracks);
    writeArray(context, _patterns);

    writer.write(_selectedTrackIndex);
    writer.write(_selectedPatternIndex);
}

void Project::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_bpm);
    reader.read(_swing);

    _clockSetup.read(context);

    readArray(context, _tracks);
    readArray(context, _patterns);

    reader.read(_selectedTrackIndex);
    reader.read(_selectedPatternIndex);
}
