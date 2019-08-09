#pragma once

#include "Config.h"

#include "EngineState.h"
#include "MidiPort.h"

#include "model/Model.h"

#include "core/midi/MidiMessage.h"

#include <cstdint>

class Engine;
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
    TrackEngine(Engine &engine, const Model &model, Track &track, const TrackEngine *linkedTrackEngine) :
        _engine(engine),
        _model(model),
        _track(track),
        _trackState(model.project().playState().trackState(track.trackIndex())),
        _linkedTrackEngine(linkedTrackEngine)
    {
        changePattern();
    }

    const TrackEngine *linkedTrackEngine() const { return _linkedTrackEngine; }
    void setLinkedTrackEngine(const TrackEngine *linkedTrackEngine) {
        _linkedTrackEngine = linkedTrackEngine;
    }

    template<typename T>
    const T &as() const {
        SANITIZE_TRACK_MODE(_track.trackMode(), trackMode());
        return *static_cast<const T *>(this);
    }

    template<typename T>
    T &as() {
        SANITIZE_TRACK_MODE(_track.trackMode(), trackMode());
        return *static_cast<T *>(this);
    }

    virtual Track::TrackMode trackMode() const = 0;

    // sequencer control

    virtual void reset() = 0;
    virtual void restart() = 0;
    virtual void tick(uint32_t tick) = 0;
    virtual void update(float dt) = 0;

    virtual void changePattern() {}

    virtual bool receiveMidi(MidiPort port, const MidiMessage &message) { return false; }
    virtual void monitorMidi(uint32_t tick, const MidiMessage &message) {}

    virtual const TrackLinkData *linkData() const { return nullptr; }

    // track output

    virtual bool activity() const = 0;
    virtual bool gateOutput(int index) const = 0;
    virtual float cvOutput(int index) const = 0;

    virtual float sequenceProgress() const { return -1.f; }

    // helpers

    bool isSelected() const { return _model.project().selectedTrackIndex() == _track.trackIndex(); }

    int swing() const { return _model.project().swing(); }

    int pattern() const { return _trackState.pattern(); }
    bool mute() const { return _trackState.mute(); }
    bool fill() const { return _trackState.fill(); }
    int fillAmount() const { return _trackState.fillAmount(); }

protected:
    Engine &_engine;
    const Model &_model;
    Track &_track;
    const PlayState::TrackState &_trackState;
    const TrackEngine *_linkedTrackEngine;
};

#undef SANITIZE_TRACK_MODE
