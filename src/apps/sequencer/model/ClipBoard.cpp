#include "ClipBoard.h"

ClipBoard::ClipBoard() {
    clear();
}

void ClipBoard::clear() {
    _type = Type::None;
}

void ClipBoard::copyTrack(const Track &track) {
    _type = Type::Track;
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
}

void ClipBoard::pasteTrack(Track &track) const {
    if (canPasteTrack()) {
    }
}

void ClipBoard::pasteNoteSequence(NoteSequence &noteSequence) const {
    if (canPasteNoteSequence()) {
        noteSequence = _container.as<NoteSequence>();
    }
}

void ClipBoard::pasteCurveSequence(CurveSequence &curveSequence) const {
    if (canPasteCurveSequence()) {
        curveSequence = _container.as<CurveSequence>();
    }
}

void ClipBoard::pastePattern(Project &project, int patternIndex) const {
    if (canPastePattern()) {

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
