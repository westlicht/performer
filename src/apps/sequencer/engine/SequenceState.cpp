#include "SequenceState.h"

#include "core/Debug.h"
#include "core/math/Math.h"

static int randomStep(int firstStep, int lastStep, Random &rng) {
    return rng.nextRange(lastStep - firstStep + 1) + firstStep;
}

void SequenceState::reset() {
    _step = -1;
    _lastStep = -1;
    _direction = 1;
}

void SequenceState::advanceFree(Types::RunMode runMode, int firstStep, int lastStep, Random &rng) {
     ASSERT(firstStep <= lastStep, "invalid first/last step");

   _lastStep = _step;

    if (_step == -1) {
        // first step
        switch (runMode) {
        case Types::RunMode::Forward:
        case Types::RunMode::Pendulum:
        case Types::RunMode::PingPong:
            _step = firstStep;
            break;
        case Types::RunMode::Backward:
            _step = lastStep;
            break;
        case Types::RunMode::Random:
        case Types::RunMode::RandomWalk:
            _step = randomStep(firstStep, lastStep, rng);
            break;
        case Types::RunMode::Last:
            break;
        }
    } else {
        // advance step
        _step = clamp(int(_step), firstStep, lastStep);

        switch (runMode) {
        case Types::RunMode::Forward:
            _step = _step >= lastStep ? firstStep : _step + 1;
            break;
        case Types::RunMode::Backward:
            _step = _step <= firstStep ? lastStep : _step - 1;
            break;
        case Types::RunMode::Pendulum:
        case Types::RunMode::PingPong:
            if (_direction > 0 && _step >= lastStep) {
                _direction = -1;
            } else if (_direction < 0 && _step <= firstStep) {
                _direction = 1;
            } else {
                if (runMode == Types::RunMode::Pendulum) {
                    _step += _direction;
                }
            }
            if (runMode == Types::RunMode::PingPong) {
                _step += _direction;
            }
            break;
        case Types::RunMode::Random:
            _step = randomStep(firstStep, lastStep, rng);
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
     ASSERT(firstStep <= lastStep, "invalid first/last step");

    _lastStep = _step;

    int stepCount = lastStep - firstStep + 1;

    switch (runMode) {
    case Types::RunMode::Forward:
        _step = firstStep + absoluteStep % stepCount;
        break;
    case Types::RunMode::Pendulum:
        absoluteStep %= 2 * stepCount;
        _step = (absoluteStep < stepCount) ? (firstStep + absoluteStep) : (lastStep - (absoluteStep - stepCount));
        break;
    case Types::RunMode::PingPong:
        absoluteStep %= 2 * stepCount - 2;
        _step = (absoluteStep < stepCount) ? (firstStep + absoluteStep) : (lastStep - (absoluteStep - stepCount) - 1);
        break;
    case Types::RunMode::Backward:
        _step = lastStep - absoluteStep % stepCount;
        break;
    case Types::RunMode::Random:
        _step = firstStep + rng.nextRange(stepCount);
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
        _step = randomStep(firstStep, lastStep, rng);
    } else {
        int dir = rng.nextRange(2);
        if (dir == 0) {
            _step = _step == firstStep ? lastStep : _step - 1;
        } else {
            _step = _step == lastStep ? firstStep : _step + 1;
        }
    }
}
