#include "Project.h"

#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

void Project::setTrackSetup(int index, const TrackSetup &trackSetup) {
    bool modeChanged = trackSetup.mode() != _trackSetups[index].mode();
    _trackSetups[index] = trackSetup;
    if (modeChanged) {
        // TODO reset snapshots
        for (auto &pattern : _patterns) {
            auto &sequence = pattern.sequence(index);
            switch (trackSetup.mode()) {
            case TrackSetup::Mode::Note:    sequence.noteSequence().setDefault(); break;
            case TrackSetup::Mode::Curve:   sequence.curveSequence().setDefault(); break;
            case TrackSetup::Mode::Last:    break;
            }
        }
    }
}

void Project::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_name, NameLength + 1);
    writer.write(_bpm);
    writer.write(_swing);
    writer.write(_globalMeasure);

    _clockSetup.write(context);
    writeArray(context, _trackSetups);
    _playState.write(context);
    writeArray(context, _patterns);

    writer.write(_selectedTrackIndex);
    writer.write(_selectedPatternIndex);
}

void Project::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_name, NameLength + 1);
    reader.read(_bpm);
    reader.read(_swing);
    reader.read(_globalMeasure);

    _clockSetup.read(context);
    readArray(context, _trackSetups);
    _playState.read(context);
    readArray(context, _patterns);

    reader.read(_selectedTrackIndex);
    reader.read(_selectedPatternIndex);
}

fs::Error Project::write(const char *path) const {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        fileWriter.error();
    }

    ProjectWriter projectWriter(fileWriter);
    WriteContext context = { *this, projectWriter };

    write(context);

    return fileWriter.finish();
}

fs::Error Project::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        fileReader.error();
    }

    ProjectReader projectReader(fileReader);
    ReadContext context = { *this, projectReader };

    read(context);

    return fileReader.finish();
}
