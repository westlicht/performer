#include "ClipBoard.h"

#include "Model.h"

ClipBoard::ClipBoard() {
    clear();
}

void ClipBoard::clear() {
    _type = Type::None;
}

void ClipBoard::copyTrack(const Track &track) {
    _type = Type::Track;
    _container.as<Track>() = track;
}

void ClipBoard::copyNoteSequence(const NoteSequence &noteSequence) {
    _type = Type::NoteSequence;
    _container.as<NoteSequence>() = noteSequence;
}

void ClipBoard::copyCurveSequence(const CurveSequence &curveSequence) {
    _type = Type::CurveSequence;
    _container.as<CurveSequence>() = curveSequence;
}

void ClipBoard::copyPattern(Project &project, int patternIndex) {
    _type = Type::Pattern;
    auto &pattern = _container.as<Pattern>();
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &track = project.track(trackIndex);
        pattern.sequences[trackIndex].trackMode = track.trackMode();
        switch (track.trackMode()) {
        case Track::TrackMode::Note:
            pattern.sequences[trackIndex].data.note = track.noteTrack().sequence(patternIndex);
            break;
        case Track::TrackMode::Curve:
            pattern.sequences[trackIndex].data.curve = track.curveTrack().sequence(patternIndex);
            break;
        default:
            break;
        }
    }
}

void ClipBoard::pasteTrack(Track &track) const {
    if (canPasteTrack()) {
        Model::ConfigLock lock;
        track = _container.as<Track>();
    }
}

void ClipBoard::pasteNoteSequence(NoteSequence &noteSequence) const {
    if (canPasteNoteSequence()) {
        Model::WriteLock lock;
        noteSequence = _container.as<NoteSequence>();
    }
}

void ClipBoard::pasteCurveSequence(CurveSequence &curveSequence) const {
    if (canPasteCurveSequence()) {
        Model::WriteLock lock;
        curveSequence = _container.as<CurveSequence>();
    }
}

void ClipBoard::pastePattern(Project &project, int patternIndex) const {
    if (canPastePattern()) {
        Model::WriteLock lock;
        const auto &pattern = _container.as<Pattern>();
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            auto &track = project.track(trackIndex);
            if (track.trackMode() == pattern.sequences[trackIndex].trackMode) {
                switch (track.trackMode()) {
                case Track::TrackMode::Note:
                    track.noteTrack().sequence(patternIndex) = pattern.sequences[trackIndex].data.note;
                    break;
                case Track::TrackMode::Curve:
                    track.curveTrack().sequence(patternIndex) = pattern.sequences[trackIndex].data.curve;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

bool ClipBoard::canPasteTrack() const {
    return _type == Type::Track;
}

bool ClipBoard::canPasteNoteSequence() const {
    return _type == Type::NoteSequence;
}

bool ClipBoard::canPasteCurveSequence() const {
    return _type == Type::CurveSequence;
}

bool ClipBoard::canPastePattern() const {
    return _type == Type::Pattern;
}
