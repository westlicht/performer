#include "Track.h"

#include "core/Debug.h"
#include "core/utils/Random.h"

static Random rng;

Track::Track() {
    reset();
}

void Track::reset() {
    _currentStep = -1;
    _direction = 1;
    _gate = false;
}

void Track::tick(uint32_t tick) {
    const auto &sequence = *_sequence;

    if (tick % (192 / 4) == 0) {
        advance(sequence);
        _gate = !_muted && _sequence->step(_currentStep).active;
    }
    if (tick % (192 / 4) >= (192 / 8)) {
        _gate = false;
    }
}

void Track::advance(const Sequence &sequence) {
    auto playMode = sequence.playMode();
    int firstStep = sequence.firstStep();
    int lastStep = sequence.lastStep();
    ASSERT(firstStep <= lastStep, "invalid first/last step");

    auto randomStep = [&] () {
        return rng.next() % (lastStep - firstStep + 1) + firstStep;
    };

    // first step
    if (_currentStep == -1) {
        switch (playMode) {
        case Sequence::Forward:
        case Sequence::PingPong:
        case Sequence::Pendulum:
            _currentStep = firstStep;
            break;
        case Sequence::Backward:
            _currentStep = lastStep;
            break;
        case Sequence::Random:
            _currentStep = randomStep();
            break;
        }
    } else {
        switch (playMode) {
        case Sequence::Forward:
            _currentStep = _currentStep >= lastStep ? firstStep : _currentStep + 1;
            break;
        case Sequence::Backward:
            _currentStep = _currentStep <= firstStep ? lastStep : _currentStep - 1;
            break;
        case Sequence::PingPong:
        case Sequence::Pendulum:
            if (_direction > 0 && _currentStep >= lastStep) {
                _direction = -1;
            } else if (_direction < 0 && _currentStep <= firstStep) {
                _direction = 1;
            } else {
                if (playMode == Sequence::Pendulum) {
                    _currentStep += _direction;
                }
            }
            if (playMode == Sequence::PingPong) {
                _currentStep += _direction;
            }
            break;
        case Sequence::Random:
            _currentStep = randomStep();
            break;
        }
    }
}
