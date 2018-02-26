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

    const uint32_t divisor = (CONFIG_PPQN / 4);

    if (tick % divisor == 0) {
        advance();

        // const auto &scale = Scale::get(sequence.scale());
        // _cvOutput = evalStepNote(step, scale);
    }

    _currentStepFraction = float(tick % divisor) / divisor;

    float value = evalStepShape(sequence.step(_currentStep), _currentStepFraction);
    // value = _range[0] + value * (_range[1] - _range[0]);
    _cvOutput = value;
}

void CurveSequenceEngine::setSequence(const Sequence &sequence) {
    _sequence = &sequence.curveSequence();
}

void CurveSequenceEngine::setMute(bool mute) {
    _mute = mute;
}

void CurveSequenceEngine::setFill(bool fill) {

}

void CurveSequenceEngine::advance() {
    const auto &sequence = *_sequence;

    auto playMode = sequence.playMode();
    int firstStep = sequence.firstStep();
    int lastStep = sequence.lastStep();
    ASSERT(firstStep <= lastStep, "invalid first/last step");

    auto randomStep = [&] () {
        return rng.nextRange(lastStep - firstStep + 1) + firstStep;
    };

    if (_currentStep == -1) {
        // first step
        switch (playMode) {
        case CurveSequence::PlayMode::Forward:
        case CurveSequence::PlayMode::PingPong:
        case CurveSequence::PlayMode::Pendulum:
            _currentStep = firstStep;
            break;
        case CurveSequence::PlayMode::Backward:
            _currentStep = lastStep;
            break;
        case CurveSequence::PlayMode::Random:
            _currentStep = randomStep();
            break;
        case CurveSequence::PlayMode::Last:
            break;
        }
    } else {
        // advance step
        switch (playMode) {
        case CurveSequence::PlayMode::Forward:
            _currentStep = _currentStep >= lastStep ? firstStep : _currentStep + 1;
            break;
        case CurveSequence::PlayMode::Backward:
            _currentStep = _currentStep <= firstStep ? lastStep : _currentStep - 1;
            break;
        case CurveSequence::PlayMode::PingPong:
        case CurveSequence::PlayMode::Pendulum:
            if (_direction > 0 && _currentStep >= lastStep) {
                _direction = -1;
            } else if (_direction < 0 && _currentStep <= firstStep) {
                _direction = 1;
            } else {
                if (playMode == CurveSequence::PlayMode::Pendulum) {
                    _currentStep += _direction;
                }
            }
            if (playMode == CurveSequence::PlayMode::PingPong) {
                _currentStep += _direction;
            }
            break;
        case CurveSequence::PlayMode::Random:
            _currentStep = randomStep();
            break;
        case CurveSequence::PlayMode::Last:
            break;
        }
    }
}
