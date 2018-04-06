#pragma once

#include "Config.h"

#include "MidiPort.h"

#include "model/Track.h"
#include "model/PlayState.h"

#include "core/midi/MidiMessage.h"

#include <cstdint>

class SequenceState;

struct TrackLinkData {
    uint32_t divisor;
    uint32_t relativeTick;
    SequenceState *sequenceState;
};

#if CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) ASSERT(_actual_ == _expected_, "invalid track mode");
#else // CONFIG_ENABLE_SANITIZE
# define SANITIZE_TRACK_MODE(_actual_, _expected_) {}
#endif // CONFIG_ENABLE_SANITIZE

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
        SANITIZE_TRACK_MODE(_trackMode, T::trackMode);
        return *static_cast<const T *>(this);
    }

    template<typename T>
    T &as() {
        SANITIZE_TRACK_MODE(_trackMode, T::trackMode);
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
    virtual void update(float dt) = 0;
    virtual void receiveMidi(MidiPort port, int channel, const MidiMessage &message) {}
    virtual void changePattern() {}

    virtual const TrackLinkData *linkData() const { return nullptr; }

    // track output

    virtual bool activity() const = 0;
    virtual bool gateOutput(int index) const = 0;
    virtual float cvOutput(int index) const = 0;

    virtual bool idleOutput() const { return false; }
    virtual bool idleGateOutput(int index) const { return false; }
    virtual float idleCvOutput(int index) const { return 0.f; }

protected:
    const Track &_track;
    Track::TrackMode _trackMode;
    const TrackEngine *_linkedTrackEngine;

    uint8_t _swing;
    uint8_t _pattern;
    bool _mute;
    bool _fill;
};

#undef SANITIZE_TRACK_MODE
