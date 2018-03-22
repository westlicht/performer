#pragma once

#include "NoteSequenceEngine.h"
#include "CurveSequenceEngine.h"

#include "model/Track.h"
#include "model/PlayState.h"

#include "core/Debug.h"
#include "core/utils/Container.h"

#include <cstdint>

class TrackEngine {
public:
    void setup(const Track &track, const TrackEngine *linkedTrackEngine);

    void reset();

    void tick(uint32_t tick);

    void setSwing(int swing);

    int patternIndex() const { return _patternIndex; }
    void setPatternIndex(int patternIndex);

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
        ASSERT(_trackMode == Track::TrackMode::Note, "invalid mode");
        return *static_cast<const NoteSequenceEngine *>(_sequenceEngine);
    }

    const CurveSequenceEngine &curveSequenceEngine() const {
        ASSERT(_trackMode == Track::TrackMode::Curve, "invalid mode");
        return *static_cast<const CurveSequenceEngine *>(_sequenceEngine);
    }

private:
    uint8_t _patternIndex = 0;
    bool _mute = false;
    bool _fill = false;

    Track::TrackMode _trackMode = Track::TrackMode::Last;
    const SequenceEngine *_linkedSequenceEngine = nullptr;

    Container<NoteSequenceEngine, CurveSequenceEngine> _sequenceEngineContainer;
    SequenceEngine *_sequenceEngine = nullptr;
};
