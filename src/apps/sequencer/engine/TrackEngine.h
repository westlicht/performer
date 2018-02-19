#pragma once

#include "NoteSequenceEngine.h"
#include "CurveSequenceEngine.h"

#include "model/TrackSetup.h"
#include "model/Pattern.h"

#include <cstdint>

class TrackEngine {
public:
    void init(int trackIndex);

    void setup(const TrackSetup &trackSetup);

    void setPattern(const Pattern &pattern);

    void reset();

    void tick(uint32_t tick);

    bool muted() const { return _muted; }
    void setMuted(bool muted) { _muted = muted; }

    // bool gate() const { return _gate; }
    bool gateOutput() const;
    float cvOutput() const;

    const NoteSequenceEngine &noteSequenceEngine() const { return _sequenceEngine.note; }
    const CurveSequenceEngine &curveSequenceEngine() const { return _sequenceEngine.curve; }

private:
    uint8_t _trackIndex;
    bool _muted;

    TrackSetup::Mode _mode;

    union {
        NoteSequenceEngine note;
        CurveSequenceEngine curve;
    } _sequenceEngine;
};
