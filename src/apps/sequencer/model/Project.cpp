#include "Project.h"

#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

Project::Project() :
    _playState(*this),
    _routing(*this)
{
    for (size_t i = 0; i < _tracks.size(); ++i) {
        _tracks[i].setTrackIndex(i);
    }

    clear();
}

void Project::writeRouted(Routing::Target target, int intValue, float floatValue) {
    switch (target) {
    case Routing::Target::Tempo:
        setTempo(floatValue, true);
        break;
    case Routing::Target::Swing:
        setSwing(intValue, true);
        break;
    default:
        break;
    }
}

void Project::clear() {
    _slot = uint8_t(-1);
    StringUtils::copy(_name, "INIT", sizeof(_name));
    setTempo(120.f);
    setSwing(50);
    setSyncMeasure(1);
    setScale(0);
    setRootNote(0);
    setRecordMode(Types::RecordMode::Overdub);
    setCvGateInput(Types::CvGateInput::Off);

    _routed.clear();

    _clockSetup.clear();

    for (auto &track : _tracks) {
        track.clear();
    }

    for (int i = 0; i < CONFIG_CHANNEL_COUNT; ++i) {
        _cvOutputTracks[i] = i;
        _gateOutputTracks[i] = i;
    }

    _song.clear();
    _playState.clear();
    _routing.clear();
    _midiOutput.clear();

    for (auto &userScale : UserScale::userScales) {
        userScale.clear();
    }

    setSelectedTrackIndex(0);
    setSelectedPatternIndex(0);

    // load demo project on simulator
#if PLATFORM_SIM
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
    noteSequence(7, 0).setGates({ 1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1 });
    noteSequence(7, 0).setNotes({ 0,0,0,0,12,0,12,1,24,21,22,0,3,6,12,1 });
#endif

    _observable.notify(ProjectCleared);
}

void Project::clearPattern(int patternIndex) {
    for (auto &track : _tracks) {
        track.clearPattern(patternIndex);
    }
}

void Project::setTrackMode(int trackIndex, Track::TrackMode trackMode) {
    // TODO make sure engine is synced to this before updating UI
    _playState.revertSnapshot();
    _tracks[trackIndex].setTrackMode(trackMode);
    _observable.notify(TrackModeChanged);
}

void Project::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_name, NameLength + 1);
    writer.write(_tempo.base);
    writer.write(_swing.base);
    writer.write(_syncMeasure);
    writer.write(_scale);
    writer.write(_rootNote);
    writer.write(_recordMode);
    writer.write(_cvGateInput);

    _clockSetup.write(context);

    writeArray(context, _tracks);
    writeArray(context, _cvOutputTracks);
    writeArray(context, _gateOutputTracks);

    _song.write(context);
    _playState.write(context);
    _routing.write(context);
    _midiOutput.write(context);

    writeArray(context, UserScale::userScales);

    writer.write(_selectedTrackIndex);
    writer.write(_selectedPatternIndex);

    writer.writeHash();
}

bool Project::read(ReadContext &context) {
    clear();

    auto &reader = context.reader;
    reader.read(_name, NameLength + 1, Version5);
    reader.read(_tempo.base);
    reader.read(_swing.base);
    reader.read(_syncMeasure);
    reader.read(_scale);
    reader.read(_rootNote);
    reader.read(_recordMode);
    reader.read(_cvGateInput, Version6);

    _clockSetup.read(context);

    readArray(context, _tracks);
    readArray(context, _cvOutputTracks);
    readArray(context, _gateOutputTracks);

    _song.read(context);
    _playState.read(context);
    _routing.read(context);
    _midiOutput.read(context);

    if (reader.dataVersion() >= Version5) {
        readArray(context, UserScale::userScales);
    }

    reader.read(_selectedTrackIndex);
    reader.read(_selectedPatternIndex);

    bool success = reader.checkHash();
    if (success) {
        _observable.notify(ProjectRead);
    } else {
        clear();
    }

    return success;
}

fs::Error Project::write(const char *path) const {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        return fileWriter.error();
    }

    FileHeader header(FileType::Project, 0, _name);
    fileWriter.write(&header, sizeof(header));

    VersionedSerializedWriter writer(
        [&fileWriter] (const void *data, size_t len) { fileWriter.write(data, len); },
        Version
    );

    WriteContext context = { writer };
    write(context);

    return fileWriter.finish();
}

fs::Error Project::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        return fileReader.error();
    }

    FileHeader header;
    fileReader.read(&header, sizeof(header));

    VersionedSerializedReader reader(
        [&fileReader] (void *data, size_t len) { fileReader.read(data, len); },
        Version
    );

    ReadContext context = { reader };
    bool success = read(context);

    // TODO at some point we should remove this because name is also stored with data as of version 5
    if (success) {
        header.readName(_name, sizeof(_name));
    }

    auto error = fileReader.finish();
    if (error == fs::OK && !success) {
        error = fs::INVALID_CHECKSUM;
    }

    return error;
}
