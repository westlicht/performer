#include "Project.h"

#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

Project::Project() {
    for (size_t i = 0; i < _trackSetups.size(); ++i) {
        _trackSetups[i].setTrackIndex(i);
    }
}

void Project::clear() {
    _slot = uint8_t(-1);
    StringUtils::copy(_name, "INIT", sizeof(_name));
    _bpm = 120.f;
    _swing = 50;
    _syncMeasure = 1;

    _clockSetup.clear();

    for (auto &trackSetup : _trackSetups) {
        trackSetup.clear();
    }

    for (auto &trackSequences : _sequences) {
        for (auto &sequence : trackSequences) {
            sequence.clear();
        }
    }

    _playState.clear();

    _selectedTrackIndex = 0;
    _selectedPatternIndex = 0;

    // TODO remove
    demoProject();
}

void Project::demoProject() {
    sequence(0, 0).noteSequence().setGates({ 1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 });
    sequence(1, 0).noteSequence().setGates({ 0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0 });
    sequence(2, 0).noteSequence().setGates({ 0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0 });
    sequence(3, 0).noteSequence().setGates({ 0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0 });
    sequence(4, 0).noteSequence().setGates({ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 });
    sequence(5, 0).noteSequence().setGates({ 0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0 });
    sequence(7, 0).noteSequence().setGates({ 1,0,1,0,1,0,1,1,1,1,1,0,1,1,0,1 });
    sequence(7, 0).noteSequence().setNotes({ 36,36,36,36,48,36,48,37,60,61,58,36,39,42,48,37 });

    setTrackMode(0, Types::TrackMode::Curve);
    sequence(0, 0).curveSequence().setLastStep(7);
}

void Project::setTrackSetup(int trackIndex, const TrackSetup &trackSetup) {
    // TODO make sure engine is synced to this before updating UI
    if (trackSetup.trackMode() != _trackSetups[trackIndex].trackMode()) {
        setTrackMode(trackIndex, trackSetup.trackMode());
    }
    _trackSetups[trackIndex] = trackSetup;
}

void Project::setTrackMode(int trackIndex, Types::TrackMode trackMode) {
    // TODO reset snapshots
    _trackSetups[trackIndex].setTrackMode(trackMode);
    for (auto &sequence : _sequences[trackIndex]) {
        switch (trackMode) {
        case Types::TrackMode::Note:    sequence.noteSequence().clear(); break;
        case Types::TrackMode::Curve:   sequence.curveSequence().clear(); break;
        case Types::TrackMode::Last:    break;
        }
    }
}

void Project::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_name, NameLength + 1);
    writer.write(_bpm);
    writer.write(_swing);
    writer.write(_syncMeasure);

    _clockSetup.write(context);
    writeArray(context, _trackSetups);

    for (const auto &sequenceArray : _sequences) {
        for (const auto &sequence : sequenceArray) {
            sequence.write(context);
        }
    }

    _playState.write(context);

    writer.write(_selectedTrackIndex);
    writer.write(_selectedPatternIndex);
}

void Project::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_name, NameLength + 1);
    reader.read(_bpm);
    reader.read(_swing);
    reader.read(_syncMeasure);

    _clockSetup.read(context);
    readArray(context, _trackSetups);

    for (auto &sequenceArray : _sequences) {
        for (auto &sequence : sequenceArray) {
            sequence.read(context);
        }
    }

    _playState.read(context);

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
