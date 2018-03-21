#pragma once

#include "Config.h"

#include "Track.h"
#include "NoteSequence.h"
#include "CurveSequence.h"
#include "Project.h"

#include "core/utils/Container.h"

class ClipBoard {
public:
    ClipBoard();

    void clear();

    void copyTrack(const Track &track);
    void copyNoteSequence(const NoteSequence &noteSequence);
    void copyCurveSequence(const CurveSequence &curveSequence);
    void copyPattern(Project &project, int patternIndex);

    void pasteTrack(Track &track) const;
    void pasteNoteSequence(NoteSequence &noteSequence) const;
    void pasteCurveSequence(CurveSequence &curveSequence) const;
    void pastePattern(Project &project, int patternIndex) const;

    bool canPasteTrack() const;
    bool canPasteNoteSequence() const;
    bool canPasteCurveSequence() const;
    bool canPastePattern() const;

private:
    enum class Type : uint8_t {
        None,
        Track,
        NoteSequence,
        CurveSequence,
        Pattern,
    };

    struct Pattern {
        struct {
            Types::TrackMode trackMode;
            union {
                NoteSequence note;
                CurveSequence curve;
            } data;
        } sequences[CONFIG_TRACK_COUNT];
    };

    Type _type = Type::None;
    Container<NoteSequence, CurveSequence, Pattern> _container;
};
