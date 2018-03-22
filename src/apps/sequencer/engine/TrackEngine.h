#pragma once

#include "model/Track.h"
#include "model/PlayState.h"

#include <cstdint>

class SequenceState;

struct TrackLinkData {
    uint32_t divisor;
    uint32_t relativeTick;
    SequenceState *sequenceState;
};

class TrackEngine {
public:
    TrackEngine(const Track &track, const TrackEngine *linkedTrackEngine) :
        _track(track),
        _trackMode(track.trackMode()),
        _linkedTrackEngine(linkedTrackEngine),
        _pattern(0),
        _mute(false),
        _fill(false)
    {
    }

    void setLinkedTrackEngine(const TrackEngine *linkedTrackEngine) {
        _linkedTrackEngine = linkedTrackEngine;
    }

    Track::TrackMode trackMode() const { return _trackMode; }

    template<typename T>
    const T &as() const {
        // TODO sanitze
        return *static_cast<const T *>(this);
    }

    template<typename T>
    T &as() {
        // TODO sanitize
        return *static_cast<T *>(this);
    }

    // sequencer control

    int swing() const { return _swing; }
    void setSwing(int swing);

    int pattern() const { return _pattern; }
    void setPattern(int pattern);

    bool mute() const { return _mute; }
    void setMute(bool mute);

    bool fill() const { return _fill; }
    void setFill(bool fill);

    virtual void reset() = 0;
    virtual void tick(uint32_t tick) = 0;
    virtual void changePattern() = 0;

    virtual const TrackLinkData *linkData() const { return nullptr; }

    // track output

    virtual bool gate() const = 0;
    virtual bool gateOutput() const = 0;
    virtual float cvOutput() const = 0;

protected:
    const Track &_track;
    Track::TrackMode _trackMode;
    const TrackEngine *_linkedTrackEngine;

    uint8_t _swing;
    uint8_t _pattern;
    bool _mute;
    bool _fill;
};
