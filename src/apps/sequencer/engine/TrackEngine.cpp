#include "TrackEngine.h"

#include "core/Debug.h"

void TrackEngine::setup(const Track &track, const TrackEngine *linkedTrackEngine) {
    const SequenceEngine *linkedSequenceEngine = linkedTrackEngine ? &linkedTrackEngine->sequenceEngine() : nullptr;

    if (track.trackMode() == _trackMode) {
        // update linked sequence engine
        if (_sequenceEngine && linkedSequenceEngine != _linkedSequenceEngine) {
            _sequenceEngine->setLinkedSequenceEngine(linkedSequenceEngine);
        }
        _linkedSequenceEngine = linkedSequenceEngine;
        return;
    }

    _trackMode = track.trackMode();
    _linkedSequenceEngine = linkedSequenceEngine;

    _sequenceEngineContainer.destroy(_sequenceEngine);
    ASSERT(_trackMode != Types::TrackMode::Last, "invalid track mode");
    switch (_trackMode) {
    case Types::TrackMode::Note:
        _sequenceEngine = _sequenceEngineContainer.create<NoteSequenceEngine>();
        break;
    case Types::TrackMode::Curve:
        _sequenceEngine = _sequenceEngineContainer.create<CurveSequenceEngine>();
        break;
    case Types::TrackMode::Last:
        break;
    }

    _sequenceEngine->setup(track);
    _sequenceEngine->setMute(_mute);
    _sequenceEngine->setLinkedSequenceEngine(linkedSequenceEngine);
    _sequenceEngine->setPatternIndex(_patternIndex);
}

void TrackEngine::reset() {
    _sequenceEngine->reset();
}

void TrackEngine::tick(uint32_t tick) {
    _sequenceEngine->tick(tick);
}

void TrackEngine::setSwing(int swing) {
    _sequenceEngine->setSwing(swing);
}

void TrackEngine::setPatternIndex(int patternIndex) {
    _patternIndex = patternIndex;
    _sequenceEngine->setPatternIndex(_patternIndex);
}

void TrackEngine::setMute(bool mute) {
    _mute = mute;
    _sequenceEngine->setMute(mute);
}

void TrackEngine::setFill(bool fill) {
    _fill = fill;
    _sequenceEngine->setFill(fill);
}

bool TrackEngine::gate() const {
    return _sequenceEngine->gate();
}

bool TrackEngine::gateOutput() const {
    return _sequenceEngine->gateOutput();
}

float TrackEngine::cvOutput() const {
    return _sequenceEngine->cvOutput();
}
