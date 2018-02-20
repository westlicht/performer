#include "CurveSequenceEngine.h"

void CurveSequenceEngine::setup(const TrackSetup &trackSetup) {

}

void CurveSequenceEngine::reset() {

}

void CurveSequenceEngine::tick(uint32_t tick) {

}

void CurveSequenceEngine::setSequence(const Sequence &sequence) {
    _sequence = &sequence.curveSequence();
}

void CurveSequenceEngine::setMute(bool mute) {

}
