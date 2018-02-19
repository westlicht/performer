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

    bool mute() const { return _mute; }
    void setMute(bool mute);

    bool gate() const;
    bool gateOutput() const;
    float cvOutput() const;

    const NoteSequenceEngine &noteSequenceEngine() const { return _sequenceEngine.note; }
    const CurveSequenceEngine &curveSequenceEngine() const { return _sequenceEngine.curve; }

private:
    uint8_t _trackIndex;
    bool _mute;

    TrackSetup::Mode _mode;

    union {
        NoteSequenceEngine note;
        CurveSequenceEngine curve;
    } _sequenceEngine;
};
