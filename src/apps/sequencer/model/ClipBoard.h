#pragma once

#include "Config.h"

#include "Track.h"
#include "NoteSequence.h"
#include "CurveSequence.h"
#include "Project.h"

class ClipBoard {
public:
    ClipBoard();

    void clear();

    void copyTrack(const Track &track);
    void copyNoteSequence(const NoteSequence &noteSequence);
    void copyCurveSequence(const CurveSequence &curveSequence);
    void copyPattern(Project &project, int patternIndex);

    void pasteTrack(Track &track) const;
    void pasteNoteSequence(const NoteSequence &noteSequence) const;
    void pasteCurveSequence(const CurveSequence &curveSequence) const;
    void pastePattern(Project &project, int patternIndex) const;

    bool canPasteTrack() const;
    bool canPasteNoteSequence() const;
    bool canPasteCurveSequence() const;
    bool canPastePattern() const;

private:
};
