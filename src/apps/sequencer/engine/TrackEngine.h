#pragma once

#include "NoteSequenceEngine.h"
#include "CurveSequenceEngine.h"

#include "model/TrackSetup.h"
#include "model/Pattern.h"
#include "model/PlayState.h"

#include "core/Debug.h"
#include "core/utils/Container.h"

#include <cstdint>

class TrackEngine {
public:
    void init(int trackIndex);

    void setup(const TrackSetup &trackSetup);

    void setPattern(const Pattern &pattern);

    void reset();

    void tick(uint32_t tick);

    bool mute() const { return _mute; }
    void setMute(bool mute);

    bool fill() const { return _fill; }
    void setFill(bool fill);

    bool gate() const;
    bool gateOutput() const;
    float cvOutput() const;

    const NoteSequenceEngine &noteSequenceEngine() const {
        ASSERT(_mode == TrackSetup::Mode::Note, "invalid mode");
        return *static_cast<const NoteSequenceEngine *>(_sequenceEngine);
    }
    const CurveSequenceEngine &curveSequenceEngine() const {
        ASSERT(_mode == TrackSetup::Mode::Curve, "invalid mode");
        return *static_cast<const CurveSequenceEngine *>(_sequenceEngine);
    }

private:
    uint8_t _trackIndex;
    bool _mute;
    bool _fill;
    const Pattern *_pattern;

    TrackSetup::Mode _mode;

    Container<NoteSequenceEngine, CurveSequenceEngine> _sequenceEngineContainer;
    SequenceEngine *_sequenceEngine;
};
