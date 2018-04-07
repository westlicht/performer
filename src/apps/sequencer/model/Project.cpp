#include "Project.h"

#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

Project::Project() :
    _routing(*this)
{
    for (size_t i = 0; i < _tracks.size(); ++i) {
        _tracks[i].setTrackIndex(i);
    }
}

void Project::clear() {
    _slot = uint8_t(-1);
    StringUtils::copy(_name, "INIT", sizeof(_name));
    setBpm(120.f);
    setSwing(50);
    setSyncMeasure(1);
    setScale(0);
    setRootNote(0);

    _clockSetup.clear();

    for (auto &track : _tracks) {
        track.clear();
    }

    for (int i = 0; i < CONFIG_CHANNEL_COUNT; ++i) {
        _cvOutputTracks[i] = i;
        _gateOutputTracks[i] = i;
    }

    _playState.clear();
    _routing.clear();

    setSelectedTrackIndex(0);
    setSelectedPatternIndex(0);

    // TODO remove
    demoProject();
}

void Project::clearPattern(int patternIndex) {
    for (auto &track : _tracks) {
        track.clearPattern(patternIndex);
    }
}

void Project::demoProject() {
    noteSequence(0, 0).setLastStep(15);
    noteSequence(0, 0).setGates({ 1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0 });
    noteSequence(1, 0).setLastStep(15);
    noteSequence(1, 0).setGates({ 0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0 });
    noteSequence(2, 0).setLastStep(15);
    noteSequence(2, 0).setGates({ 0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0 });
    noteSequence(3, 0).setLastStep(15);
    noteSequence(3, 0).setGates({ 0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0 });
    noteSequence(4, 0).setLastStep(15);
    noteSequence(4, 0).setGates({ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 });
    noteSequence(5, 0).setLastStep(15);
    noteSequence(5, 0).setGates({ 0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0 });
    noteSequence(7, 0).setLastStep(15);
    noteSequence(7, 0).setGates({ 1,0,1,0,1,0,1,1,1,1,1,0,1,1,0,1 });
    noteSequence(7, 0).setNotes({ 36,36,36,36,48,36,48,37,60,61,58,36,39,42,48,37 });

    // setTrackMode(0, Track::TrackMode::Curve);
    // sequence(0, 0).curveSequence().setLastStep(7);

    setTrackMode(7, Track::TrackMode::MidiCv);
    for (int i = 4; i < 8; ++i) {
        setGateOutputTrack(i, 7);
        setCvOutputTrack(i, 7);
    }
}

void Project::setTrackMode(int trackIndex, Track::TrackMode trackMode) {
    // TODO make sure engine is synced to this before updating UI
    // TODO reset snapshots
    _tracks[trackIndex].setTrackMode(trackMode);
}

void Project::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_name, NameLength + 1);
    writer.write(_bpm);
    writer.write(_swing);
    writer.write(_syncMeasure);
    writer.write(_scale);
    writer.write(_rootNote);

    _clockSetup.write(context);

    writeArray(context, _tracks);
    writeArray(context, _cvOutputTracks);
    writeArray(context, _gateOutputTracks);

    _playState.write(context);
    _routing.write(context);

    writer.write(_selectedTrackIndex);
    writer.write(_selectedPatternIndex);
}

void Project::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_name, NameLength + 1);
    reader.read(_bpm);
    reader.read(_swing);
    reader.read(_syncMeasure);
    reader.read(_scale);
    reader.read(_rootNote);

    _clockSetup.read(context);

    readArray(context, _tracks);
    readArray(context, _cvOutputTracks);
    readArray(context, _gateOutputTracks);

    _playState.read(context);
    _routing.read(context);

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
