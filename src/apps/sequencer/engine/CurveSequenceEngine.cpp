#include "CurveSequenceEngine.h"

#include "Curve.h"

#include "core/Debug.h"
#include "core/utils/Random.h"
#include "core/math/Math.h"

static Random rng;

static float evalStepShape(const CurveSequence::Step &step, float fraction) {
    auto function = Curve::function(Curve::Type(step.shape()));
    float value = function(fraction);
    float min = float(step.min()) / CurveSequence::Min::Max;
    float max = float(step.max()) / CurveSequence::Max::Max;
    return min + value * (max - min);
}

void CurveSequenceEngine::setup(const TrackSetup &trackSetup) {
    _trackSetup = &trackSetup;
    reset();
}

void CurveSequenceEngine::reset() {
    _sequenceState.reset();
    _stepFraction = 0.f;
    _lastRange = CurveSequence::Range::Last;
}

void CurveSequenceEngine::tick(uint32_t tick) {
    ASSERT(_sequence != nullptr, "invalid sequence");
    const auto &sequence = *_sequence;

    // update range values
    if (sequence.range() != _lastRange) {
        _range = CurveSequence::rangeValues(sequence.range());
        _lastRange = sequence.range();
    }

    const uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);

    if (tick % divisor == 0) {
        // advance sequence
        switch (_trackSetup->playMode()) {
        case TrackSetup::PlayMode::Free:
            _sequenceState.advanceFree(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
            break;
        case TrackSetup::PlayMode::Aligned:
            _sequenceState.advanceAligned(tick / divisor, sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
            break;
        case TrackSetup::PlayMode::Last:
            break;
        }
    }

    _stepFraction = float(tick % divisor) / divisor;

    float value = evalStepShape(sequence.step(_sequenceState.step()), _stepFraction);
    value = _range[0] + value * (_range[1] - _range[0]);
    _cvOutput = value;
}

void CurveSequenceEngine::setSequence(const Sequence &sequence) {
    _sequence = &sequence.curveSequence();
}

void CurveSequenceEngine::setSwing(int swing) {

}

void CurveSequenceEngine::setMute(bool mute) {
    _mute = mute;
}

void CurveSequenceEngine::setFill(bool fill) {

}
