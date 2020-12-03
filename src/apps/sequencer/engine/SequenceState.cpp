#include "SequenceState.h"

#include "core/Debug.h"
#include "core/math/Math.h"

static int randomStep(int firstStep, int lastStep, Random &rng) {
    return rng.nextRange(lastStep - firstStep + 1) + firstStep;
}

void SequenceState::reset() {
    _nextStep = -1;
    _step = -1;
    _prevStep = -1;
    _direction = 1;
    _iteration = 0;
    _nextIteration = 0;
}

void SequenceState::advanceFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng) {
     ASSERT(firstStep <= lastStep, "invalid first/last step");

    if (_nextStep < 0) {
        calculateNextStepFree(runMode, firstStep, lastStep, rng);
    }

    _iteration = _nextIteration;
    _prevStep = _step;
    _step = _nextStep;
    _nextStep = -1;
}

void SequenceState::calculateNextStepFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng) {

    if (_nextStep == -1) {
        // first step
        switch (runMode) {
        case Types::RunMode::Forward:
        case Types::RunMode::Pendulum:
        case Types::RunMode::PingPong:
            _nextStep = firstStep;
            break;
        case Types::RunMode::Backward:
            _nextStep = lastStep;
            break;
        case Types::RunMode::Random:
        case Types::RunMode::RandomWalk:
            _nextStep = randomStep(firstStep, lastStep, rng);
            break;
        case Types::RunMode::Last:
            break;
        }
    } else {
        // advance step
        _nextStep = clamp(int(_step), firstStep, lastStep);

        switch (runMode) {
        case Types::RunMode::Forward:
            if (_step >= lastStep) {
                _nextStep = firstStep;
                ++_nextIteration;
            } else {
                ++_nextStep;
            }
            break;
        case Types::RunMode::Backward:
            if (_step <= firstStep) {
                _nextStep = lastStep;
                ++_nextIteration;
            } else {
                --_nextStep;
            }
            break;
        case Types::RunMode::Pendulum:
        case Types::RunMode::PingPong:
            if (_direction > 0 && _step>= lastStep) {
                _direction = -1;
            } else if (_direction < 0 && _step <= firstStep) {
                _direction = 1;
                ++_nextIteration;
            } else {
                if (runMode == Types::RunMode::Pendulum) {
                    _nextStep += _direction;
                }
            }
            if (runMode == Types::RunMode::PingPong) {
                _nextStep += _direction;
            }
            break;
        case Types::RunMode::Random:
            _nextStep = randomStep(firstStep, lastStep, rng);
            break;
        case Types::RunMode::RandomWalk:
            advanceRandomWalk(firstStep, lastStep, rng);
            break;
        case Types::RunMode::Last:
            break;
        }
    }
}

void SequenceState::advanceAligned(int absoluteStep, Types::RunMode runMode, int firstStep, int lastStep, Random &rng) {

    if (_nextStep < 0) {
        calculateNextStepAligned(absoluteStep, runMode, firstStep, lastStep, rng);
    }

    _iteration = _nextIteration;
    _prevStep = _step;
    _step = _nextStep;
    _nextStep = -1;
}

void SequenceState::calculateNextStepAligned(int absoluteStep, Types::RunMode runMode, int firstStep, int lastStep, Random &rng) {
     ASSERT(firstStep <= lastStep, "invalid first/last step");

    int stepCount = lastStep - firstStep + 1;

    switch (runMode) {
    case Types::RunMode::Forward:
         _nextStep = firstStep + absoluteStep % stepCount;
        break;
    case Types::RunMode::Backward:
        _nextStep = lastStep - absoluteStep % stepCount;
        _nextIteration = absoluteStep / stepCount;
        break;
    case Types::RunMode::Pendulum:
        _nextIteration = absoluteStep / (2 * stepCount);
        absoluteStep %= (2 * stepCount);
        _nextStep = (absoluteStep < stepCount) ? (firstStep + absoluteStep) : (lastStep - (absoluteStep - stepCount));
        break;
    case Types::RunMode::PingPong:
        _nextIteration = absoluteStep / (2 * stepCount - 2);
        absoluteStep %= (2 * stepCount - 2);
        _nextStep = (absoluteStep < stepCount) ? (firstStep + absoluteStep) : (lastStep - (absoluteStep - stepCount) - 1);
        break;
    case Types::RunMode::Random:
        _nextStep = firstStep + rng.nextRange(stepCount);
        break;
    case Types::RunMode::RandomWalk:
        advanceRandomWalk(firstStep, lastStep, rng);
        break;
    case Types::RunMode::Last:
        break;
    }
}

void SequenceState::advanceRandomWalk(int firstStep, int lastStep, Random &rng) {
    if (_step == -1) {
        _nextStep = randomStep(firstStep, lastStep, rng);
    } else {
        int dir = rng.nextRange(2);
        if (dir == 0) {
            _nextStep = _step == firstStep ? lastStep : _step - 1;
        } else {
            _nextStep = _step == lastStep ? firstStep : _step + 1;
        }
    }
}
