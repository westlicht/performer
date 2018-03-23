#pragma once

#include "TrackEngine.h"
#include "SequenceState.h"

#include "model/Track.h"

class MidiCvTrackEngine : public TrackEngine {
public:
    MidiCvTrackEngine(const Track &track, const TrackEngine *linkedTrackEngine) :
        TrackEngine(track, linkedTrackEngine)
    {}

    virtual void reset() override;
    virtual void tick(uint32_t tick) override;
    virtual void changePattern() override;

    virtual bool gate() const override { return false; }
    virtual bool gateOutput() const override { return false; }
    virtual float cvOutput() const override { return _cvOutput; }

    static constexpr Track::TrackMode trackMode = Track::TrackMode::MidiCv;

private:
    float _cvOutput;
};
