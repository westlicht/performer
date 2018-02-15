#include "Project.h"

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
    writer.write(_bpm);
    writer.write(_swing);

    _clockSetup.write(context);

    writeArray(context, _trackSetups);
    writeArray(context, _patterns);

    writer.write(_selectedTrackIndex);
    writer.write(_selectedPatternIndex);
}

void Project::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_bpm);
    reader.read(_swing);

    _clockSetup.read(context);

    readArray(context, _trackSetups);
    readArray(context, _patterns);

    reader.read(_selectedTrackIndex);
    reader.read(_selectedPatternIndex);
}
