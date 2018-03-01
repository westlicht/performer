#include "TrackEngine.h"

#include "core/Debug.h"

void TrackEngine::init(int trackIndex) {
    _trackMode = Types::TrackMode::Last;
    _trackIndex = trackIndex;
    _mute = false;
    _sequenceEngine = nullptr;
    _sequence = nullptr;
}

void TrackEngine::setup(const TrackSetup &trackSetup) {
    if (trackSetup.trackMode() == _trackMode) {
        return;
    }
    _trackMode = trackSetup.trackMode();
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

    _sequenceEngine->setup(trackSetup);
    _sequenceEngine->setMute(_mute);
    _sequenceEngine->setSequence(*_sequence);
}

void TrackEngine::setSequence(const Sequence &sequence) {
    _sequence = &sequence;
    _sequenceEngine->setSequence(sequence);
}

void TrackEngine::reset() {
    _sequenceEngine->reset();
}

void TrackEngine::tick(uint32_t tick) {
    _sequenceEngine->tick(tick);
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
