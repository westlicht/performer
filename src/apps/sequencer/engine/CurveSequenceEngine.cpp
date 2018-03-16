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
    reset();
}

void CurveSequenceEngine::reset() {
    _currentStep = -1;
    _currentStepFraction = 0.f;
    _direction = 1;
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
        advance();
    }

    _currentStepFraction = float(tick % divisor) / divisor;

    float value = evalStepShape(sequence.step(_currentStep), _currentStepFraction);
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

void CurveSequenceEngine::advance() {
    const auto &sequence = *_sequence;

    auto runMode = sequence.runMode();
    int firstStep = sequence.firstStep();
    int lastStep = sequence.lastStep();
    ASSERT(firstStep <= lastStep, "invalid first/last step");

    auto randomStep = [&] () {
        return rng.nextRange(lastStep - firstStep + 1) + firstStep;
    };

    if (_currentStep == -1) {
        // first step
        switch (runMode) {
        case Types::RunMode::Forward:
        case Types::RunMode::PingPong:
        case Types::RunMode::Pendulum:
            _currentStep = firstStep;
            break;
        case Types::RunMode::Backward:
            _currentStep = lastStep;
            break;
        case Types::RunMode::Random:
            _currentStep = randomStep();
            break;
        case Types::RunMode::Last:
            break;
        }
    } else {
        // advance step
        switch (runMode) {
        case Types::RunMode::Forward:
            _currentStep = _currentStep >= lastStep ? firstStep : _currentStep + 1;
            break;
        case Types::RunMode::Backward:
            _currentStep = _currentStep <= firstStep ? lastStep : _currentStep - 1;
            break;
        case Types::RunMode::PingPong:
        case Types::RunMode::Pendulum:
            if (_direction > 0 && _currentStep >= lastStep) {
                _direction = -1;
            } else if (_direction < 0 && _currentStep <= firstStep) {
                _direction = 1;
            } else {
                if (runMode == Types::RunMode::Pendulum) {
                    _currentStep += _direction;
                }
            }
            if (runMode == Types::RunMode::PingPong) {
                _currentStep += _direction;
            }
            break;
        case Types::RunMode::Random:
            _currentStep = randomStep();
            break;
        case Types::RunMode::Last:
            break;
        }
    }
}
