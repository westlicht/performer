#pragma once

#include "NoteSequenceEngine.h"
#include "CurveSequenceEngine.h"

#include "model/TrackSetup.h"
#include "model/Sequence.h"
#include "model/PlayState.h"

#include "core/Debug.h"
#include "core/utils/Container.h"

#include <cstdint>

class TrackEngine {
public:
    void setup(const TrackSetup &trackSetup, const TrackEngine *linkedTrackEngine);

    void setSequence(const Sequence &sequence);

    void reset();

    void tick(uint32_t tick);

    void setSwing(int swing);

    bool mute() const { return _mute; }
    void setMute(bool mute);

    bool fill() const { return _fill; }
    void setFill(bool fill);

    bool gate() const;
    bool gateOutput() const;
    float cvOutput() const;

    const SequenceEngine &sequenceEngine() const {
        return *_sequenceEngine;
    }

    const NoteSequenceEngine &noteSequenceEngine() const {
        ASSERT(_trackMode == Types::TrackMode::Note, "invalid mode");
        return *static_cast<const NoteSequenceEngine *>(_sequenceEngine);
    }
    const CurveSequenceEngine &curveSequenceEngine() const {
        ASSERT(_trackMode == Types::TrackMode::Curve, "invalid mode");
        return *static_cast<const CurveSequenceEngine *>(_sequenceEngine);
    }

private:
    bool _mute = false;
    bool _fill = false;

    Types::TrackMode _trackMode = Types::TrackMode::Last;
    const SequenceEngine *_linkedSequenceEngine = nullptr;

    Container<NoteSequenceEngine, CurveSequenceEngine> _sequenceEngineContainer;
    SequenceEngine *_sequenceEngine = nullptr;
    const Sequence *_sequence = nullptr;
};
