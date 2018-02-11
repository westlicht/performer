#include "Track.h"

#include "Scale.h"

#include "core/Debug.h"
#include "core/utils/Random.h"
#include "core/math/Math.h"

static Random rng;

// evaluate if step gate is active
static bool evalStepGate(const NoteSequence::Step &step) {
    return step.gate() &&
        (step.gateProbability() == NoteSequence::GateProbability::Max ||
         int(rng.nextRange(NoteSequence::GateProbability::Range)) <= step.gateProbability());
}

// evaluate step length
static int evalStepLength(const NoteSequence::Step &step) {
    int length = step.length() + 1;
    if (step.lengthVariationProbability() > 0) {
        if (int(rng.nextRange(NoteSequence::LengthVariationProbability::Range)) < step.lengthVariationProbability()) {
            int offset = step.lengthVariationRange() == 0 ? 0 : rng.nextRange(std::abs(step.lengthVariationRange()));
            if (step.lengthVariationRange() < 0) {
                offset = -offset;
            }
            length = clamp(length + offset, 0, NoteSequence::Length::Range);
        }
    }
    return length;
}

static float evalStepNote(const NoteSequence::Step &step, const Scale &scale) {
    return scale.noteVolts(step.note());
}

TrackEngine::TrackEngine() {
    reset();
}

void TrackEngine::reset() {
    _currentStep = -1;
    _direction = 1;
    _gate = false;
    _gateOutput = false;
}

void TrackEngine::tick(uint32_t tick) {
    const auto &sequence = *_sequence;
    const auto &noteSequence = sequence.noteSequence();

    const uint32_t divisor = (CONFIG_PPQN / 4);

    if (tick % divisor == 0) {
        advance(sequence);
        const auto &step = noteSequence.step(_currentStep);
        if (evalStepGate(step)) {
            _gateQueue.push({ tick, true });
            _gateQueue.push({ tick + (divisor * evalStepLength(step)) / NoteSequence::Length::Range, false });
        }

        const auto &scale = Scale::scale(noteSequence.scale());
        _cv = evalStepNote(step, scale);
    }
    while (!_gateQueue.empty() && tick >= _gateQueue.front().first) {
        _gate = _gateQueue.front().second;
        _gateOutput = !_muted && _gate;
        _gateQueue.pop();
    }
}

void TrackEngine::advance(const Sequence &sequence) {
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
        // advance step
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
