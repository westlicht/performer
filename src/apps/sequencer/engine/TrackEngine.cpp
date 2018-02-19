#include "TrackEngine.h"

#include "core/Debug.h"

void TrackEngine::init(int trackIndex) {
    _trackIndex = trackIndex;
}

void TrackEngine::setup(const TrackSetup &trackSetup) {
    _mode = trackSetup.mode();
    ASSERT(_mode != TrackSetup::Mode::Last, "invalid track mode");
    switch (_mode) {
    case TrackSetup::Mode::Note:
        _sequenceEngine.note.setup(trackSetup);
        break;
    case TrackSetup::Mode::Curve:
        _sequenceEngine.curve.setup(trackSetup);
        break;
    case TrackSetup::Mode::Last:
        break;
    }
}

void TrackEngine::setPattern(const Pattern &pattern) {
    ASSERT(_mode != TrackSetup::Mode::Last, "invalid track mode");
    switch (_mode) {
    case TrackSetup::Mode::Note:
        _sequenceEngine.note.setSequence(pattern.sequence(_trackIndex).noteSequence());
        break;
    case TrackSetup::Mode::Curve:
        _sequenceEngine.curve.setSequence(pattern.sequence(_trackIndex).curveSequence());
        break;
    case TrackSetup::Mode::Last:
        break;
    }
}

void TrackEngine::reset() {
    ASSERT(_mode != TrackSetup::Mode::Last, "invalid track mode");
    switch (_mode) {
    case TrackSetup::Mode::Note:
        _sequenceEngine.note.reset();
        break;
    case TrackSetup::Mode::Curve:
        _sequenceEngine.curve.reset();
        break;
    case TrackSetup::Mode::Last:
        break;
    }
}

void TrackEngine::tick(uint32_t tick) {
    ASSERT(_mode != TrackSetup::Mode::Last, "invalid track mode");
    switch (_mode) {
    case TrackSetup::Mode::Note:
        _sequenceEngine.note.tick(tick);
        break;
    case TrackSetup::Mode::Curve:
        _sequenceEngine.curve.tick(tick);
        break;
    case TrackSetup::Mode::Last:
        break;
    }
}

bool TrackEngine::gateOutput() const {
    ASSERT(_mode != TrackSetup::Mode::Last, "invalid track mode");
    switch (_mode) {
    case TrackSetup::Mode::Note:
        return _sequenceEngine.note.gateOutput();
    case TrackSetup::Mode::Curve:
        return _sequenceEngine.curve.gateOutput();
    case TrackSetup::Mode::Last:
        return false;
    }
}

float TrackEngine::cvOutput() const {
    ASSERT(_mode != TrackSetup::Mode::Last, "invalid track mode");
    switch (_mode) {
    case TrackSetup::Mode::Note:
        return _sequenceEngine.note.cvOutput();
    case TrackSetup::Mode::Curve:
        return _sequenceEngine.curve.cvOutput();
    case TrackSetup::Mode::Last:
        return 0.f;
    }
}
