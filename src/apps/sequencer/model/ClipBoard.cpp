#include "ClipBoard.h"

ClipBoard::ClipBoard() {
    clear();
}

void ClipBoard::clear() {
}

void ClipBoard::copyTrack(const Track &track) {

}

void ClipBoard::copyNoteSequence(const NoteSequence &noteSequence) {

}

void ClipBoard::copyCurveSequence(const CurveSequence &curveSequence) {

}

void ClipBoard::copyPattern(Project &project, int patternIndex) {

}

void ClipBoard::pasteTrack(Track &track) const {

}

void ClipBoard::pasteNoteSequence(const NoteSequence &noteSequence) const {

}

void ClipBoard::pasteCurveSequence(const CurveSequence &curveSequence) const {

}

void ClipBoard::pastePattern(Project &project, int patternIndex) const {

}

bool ClipBoard::canPasteTrack() const {
    return false;
}

bool ClipBoard::canPasteNoteSequence() const {
    return false;
}

bool ClipBoard::canPasteCurveSequence() const {
    return false;
}

bool ClipBoard::canPastePattern() const {
    return false;
}

