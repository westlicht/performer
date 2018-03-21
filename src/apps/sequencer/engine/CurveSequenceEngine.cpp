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

void CurveSequenceEngine::setup(const Track &track) {
    _track = &track;
    reset();
}

void CurveSequenceEngine::setSequence(const Sequence &sequence) {
    _sequence = &sequence.curveSequence();
}

void CurveSequenceEngine::reset() {
    _sequenceState.reset();
    _stepFraction = 0.f;
    _lastRange = CurveSequence::Range::Last;
}

void CurveSequenceEngine::tick(uint32_t tick) {
    ASSERT(_sequence != nullptr, "invalid sequence");
    const auto &sequence = *_sequence;
    const auto *sequenceLinkData = _linkedSequenceEngine ? _linkedSequenceEngine->sequenceLinkData() : nullptr;

    // update range values
    if (sequence.range() != _lastRange) {
        _range = CurveSequence::rangeValues(sequence.range());
        _lastRange = sequence.range();
    }

    if (sequenceLinkData) {
        _sequenceLinkData = *sequenceLinkData;
        _sequenceState = *sequenceLinkData->sequenceState;
        updateOutput(sequenceLinkData->relativeTick, sequenceLinkData->divisor);
    } else {
        uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
        uint32_t measureDivisor = (sequence.resetMeasure() * CONFIG_PPQN * 4);
        uint32_t relativeTick = measureDivisor == 0 ? tick : tick % measureDivisor;

        // handle reset measure
        if (relativeTick == 0) {
            reset();
        }

        relativeTick %= divisor;

        if (relativeTick == 0) {
            // advance sequence
            switch (_track->playMode()) {
            case Track::PlayMode::Free:
                _sequenceState.advanceFree(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
                break;
            case Track::PlayMode::Aligned:
                _sequenceState.advanceAligned(tick / divisor, sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
                break;
            case Track::PlayMode::Last:
                break;
            }
        }

        updateOutput(relativeTick, divisor);

        _sequenceLinkData.divisor = divisor;
        _sequenceLinkData.relativeTick = relativeTick;
        _sequenceLinkData.sequenceState = &_sequenceState;
    }
}

void CurveSequenceEngine::updateOutput(uint32_t relativeTick, uint32_t divisor) {
    const auto &sequence = *_sequence;

    _stepFraction = float(relativeTick) / divisor;

    float value = evalStepShape(sequence.step(_sequenceState.step()), _stepFraction);
    value = _range[0] + value * (_range[1] - _range[0]);
    _cvOutput = value;
}
