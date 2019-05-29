#pragma once

#include "Config.h"

#include "Track.h"
#include "NoteSequence.h"
#include "CurveSequence.h"
#include "StageSequence.h"
#include "Project.h"
#include "UserScale.h"

#include "core/utils/Container.h"

#include <bitset>

class ClipBoard {
public:
    typedef std::bitset<CONFIG_STEP_COUNT> SelectedSteps;

    ClipBoard(Project &project);

    void clear();

    void copyTrack(const Track &track);
    void copyNoteSequence(const NoteSequence &noteSequence);
    void copyNoteSequenceSteps(const NoteSequence &noteSequence, const SelectedSteps &selectedSteps);
    void copyCurveSequence(const CurveSequence &curveSequence);
    void copyCurveSequenceSteps(const CurveSequence &curveSequence, const SelectedSteps &selectedSteps);
    void copyStageSequence(const StageSequence &stageSequence);
    void copyStageSequenceSteps(const StageSequence &stageSequence, const SelectedSteps &selectedSteps);
    void copyPattern(int patternIndex);
    void copyUserScale(const UserScale &userScale);

    void pasteTrack(Track &track) const;
    void pasteNoteSequence(NoteSequence &noteSequence) const;
    void pasteNoteSequenceSteps(NoteSequence &noteSequence, const SelectedSteps &selectedSteps) const;
    void pasteCurveSequence(CurveSequence &curveSequence) const;
    void pasteCurveSequenceSteps(CurveSequence &curveSequence, const SelectedSteps &selectedSteps) const;
    void pasteStageSequence(StageSequence &stageSequence) const;
    void pasteStageSequenceSteps(StageSequence &stageSequence, const SelectedSteps &selectedSteps) const;
    void pastePattern(int patternIndex) const;
    void pasteUserScale(UserScale &userScale) const;

    bool canPasteTrack() const;
    bool canPasteNoteSequence() const;
    bool canPasteNoteSequenceSteps() const;
    bool canPasteCurveSequence() const;
    bool canPasteCurveSequenceSteps() const;
    bool canPasteStageSequence() const;
    bool canPasteStageSequenceSteps() const;
    bool canPastePattern() const;
    bool canPasteUserScale() const;

private:
    enum class Type : uint8_t {
        None,
        Track,
        NoteSequence,
        NoteSequenceSteps,
        CurveSequence,
        CurveSequenceSteps,
        StageSequence,
        StageSequenceSteps,
        Pattern,
        UserScale,
    };

    struct NoteSequenceSteps {
        NoteSequence sequence;
        SelectedSteps selected;
    };

    struct CurveSequenceSteps {
        CurveSequence sequence;
        SelectedSteps selected;
    };

    struct StageSequenceSteps {
        StageSequence sequence;
        SelectedSteps selected;
    };

    struct Pattern {
        struct {
            Track::TrackMode trackMode;
            union {
                NoteSequence note;
                CurveSequence curve;
                StageSequence stage;
            } data;
        } sequences[CONFIG_TRACK_COUNT];
    };

    Project &_project;
    Type _type = Type::None;
    Container<Track, NoteSequence, NoteSequenceSteps, CurveSequence, CurveSequenceSteps, StageSequence, Pattern, UserScale> _container;
};
