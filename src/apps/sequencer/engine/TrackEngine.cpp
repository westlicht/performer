#include "TrackEngine.h"

#include "core/Debug.h"

void TrackEngine::init(int trackIndex) {
    _mode = TrackSetup::Mode::Last;
    _trackIndex = trackIndex;
    _mute = false;
    _pattern = nullptr;
    _sequenceEngine = nullptr;
}

void TrackEngine::setup(const TrackSetup &trackSetup) {
    if (trackSetup.mode() == _mode) {
        return;
    }
    _mode = trackSetup.mode();
    _sequenceEngineContainer.destroy(_sequenceEngine);
    ASSERT(_mode != TrackSetup::Mode::Last, "invalid track mode");
    switch (_mode) {
    case TrackSetup::Mode::Note:
        _sequenceEngine = _sequenceEngineContainer.create<NoteSequenceEngine>();
        break;
    case TrackSetup::Mode::Curve:
        _sequenceEngine = _sequenceEngineContainer.create<CurveSequenceEngine>();
        break;
    case TrackSetup::Mode::Last:
        break;
    }

    _sequenceEngine->setup(trackSetup);
    _sequenceEngine->setMute(_mute);
    _sequenceEngine->setSequence(_pattern->sequence(_trackIndex));
}

void TrackEngine::setPattern(const Pattern &pattern) {
    _pattern = &pattern;
    _sequenceEngine->setSequence(_pattern->sequence(_trackIndex));
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

bool TrackEngine::gate() const {
    return _sequenceEngine->gate();
}

bool TrackEngine::gateOutput() const {
    return _sequenceEngine->gateOutput();
}

float TrackEngine::cvOutput() const {
    return _sequenceEngine->cvOutput();
}
