#include "Track.h"

void Track::clear() {
    _trackMode = TrackMode::Default;
    _linkTrack = -1;

    setupTrack();
}

void Track::clearPattern(int patternIndex) {
    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->sequence(patternIndex).clear();
        break;
    case TrackMode::Curve:
        _track.curve->sequence(patternIndex).clear();
        break;
    case TrackMode::MidiCv:
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::write(WriteContext &context) const {
    auto &writer = context.writer;
    writer.write(_trackMode);
    writer.write(_linkTrack);

    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->write(context);
        break;
    case TrackMode::Curve:
        _track.curve->write(context);
        break;
    case TrackMode::MidiCv:
        _track.midiCv->write(context);
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::read(ReadContext &context) {
    auto &reader = context.reader;
    reader.read(_trackMode);
    reader.read(_linkTrack);

    switch (_trackMode) {
    case TrackMode::Note:
        _track.note->read(context);
        break;
    case TrackMode::Curve:
        _track.curve->read(context);
        break;
    case TrackMode::MidiCv:
        _track.midiCv->read(context);
        break;
    case TrackMode::Last:
        break;
    }
}

void Track::setupTrack() {
    switch (_trackMode) {
    case TrackMode::Note:
        _track.note = _container.create<NoteTrack>();
        break;
    case TrackMode::Curve:
        _track.curve = _container.create<CurveTrack>();
        break;
    case TrackMode::MidiCv:
        _track.midiCv = _container.create<MidiCvTrack>();
        break;
    case TrackMode::Last:
        break;
    }
}
