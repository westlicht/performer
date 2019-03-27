#include "ClipBoard.h"

#include "Model.h"
#include "ModelUtils.h"

ClipBoard::ClipBoard(Project &project) :
    _project(project)
{
    clear();
}

void ClipBoard::clear() {
    _type = Type::None;
}

void ClipBoard::copyTrack(const Track &track) {
    _type = Type::Track;
    _container.as<Track>().setTrackMode(track.trackMode());
    _container.as<Track>() = track;
}

void ClipBoard::copyNoteSequence(const NoteSequence &noteSequence) {
    _type = Type::NoteSequence;
    _container.as<NoteSequence>() = noteSequence;
}

void ClipBoard::copyNoteSequenceSteps(const NoteSequence &noteSequence, const SelectedSteps &selectedSteps) {
    _type = Type::NoteSequenceSteps;
    auto &noteSequenceSteps = _container.as<NoteSequenceSteps>();
    noteSequenceSteps.sequence = noteSequence;
    noteSequenceSteps.selected = selectedSteps;
}

void ClipBoard::copyCurveSequence(const CurveSequence &curveSequence) {
    _type = Type::CurveSequence;
    _container.as<CurveSequence>() = curveSequence;
}

void ClipBoard::copyCurveSequenceSteps(const CurveSequence &curveSequence, const SelectedSteps &selectedSteps) {
    _type = Type::CurveSequenceSteps;
    auto &curveSequenceSteps = _container.as<CurveSequenceSteps>();
    curveSequenceSteps.sequence = curveSequence;
    curveSequenceSteps.selected = selectedSteps;
}

void ClipBoard::copyPattern(int patternIndex) {
    _type = Type::Pattern;
    auto &pattern = _container.as<Pattern>();
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &track = _project.track(trackIndex);
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

void ClipBoard::copyUserScale(const UserScale &userScale) {
    _type = Type::UserScale;
    _container.as<UserScale>() = userScale;
}

void ClipBoard::pasteTrack(Track &track) const {
    if (canPasteTrack()) {
        Model::ConfigLock lock;
        _project.setTrackMode(track.trackIndex(), _container.as<Track>().trackMode());
        track = _container.as<Track>();
    }
}

void ClipBoard::pasteNoteSequence(NoteSequence &noteSequence) const {
    if (canPasteNoteSequence()) {
        Model::WriteLock lock;
        noteSequence = _container.as<NoteSequence>();
    }
}

void ClipBoard::pasteNoteSequenceSteps(NoteSequence &noteSequence, const SelectedSteps &selectedSteps) {
    if (canPasteNoteSequenceSteps()) {
        const auto &noteSequenceSteps = _container.as<NoteSequenceSteps>();
        ModelUtils::copySteps(noteSequenceSteps.sequence.steps(), noteSequenceSteps.selected, noteSequence.steps(), selectedSteps);
    }
}

void ClipBoard::pasteCurveSequence(CurveSequence &curveSequence) const {
    if (canPasteCurveSequence()) {
        Model::WriteLock lock;
        curveSequence = _container.as<CurveSequence>();
    }
}

void ClipBoard::pasteCurveSequenceSteps(CurveSequence &curveSequence, const SelectedSteps &selectedSteps) {
    if (canPasteCurveSequenceSteps()) {
        const auto &curveSequenceSteps = _container.as<CurveSequenceSteps>();
        ModelUtils::copySteps(curveSequenceSteps.sequence.steps(), curveSequenceSteps.selected, curveSequence.steps(), selectedSteps);
    }
}

void ClipBoard::pastePattern(int patternIndex) const {
    if (canPastePattern()) {
        Model::WriteLock lock;
        const auto &pattern = _container.as<Pattern>();
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            auto &track = _project.track(trackIndex);
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

void ClipBoard::pasteUserScale(UserScale &userScale) const {
    if (canPasteUserScale()) {
        userScale = _container.as<UserScale>();
    }
}

bool ClipBoard::canPasteTrack() const {
    return _type == Type::Track;
}

bool ClipBoard::canPasteNoteSequence() const {
    return _type == Type::NoteSequence;
}

bool ClipBoard::canPasteNoteSequenceSteps() const {
    return _type == Type::NoteSequenceSteps;
}

bool ClipBoard::canPasteCurveSequence() const {
    return _type == Type::CurveSequence;
}

bool ClipBoard::canPasteCurveSequenceSteps() const {
    return _type == Type::CurveSequenceSteps;
}

bool ClipBoard::canPastePattern() const {
    return _type == Type::Pattern;
}

bool ClipBoard::canPasteUserScale() const {
    return _type == Type::UserScale;
}
