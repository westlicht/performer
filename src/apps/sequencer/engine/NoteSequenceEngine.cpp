#include "NoteSequenceEngine.h"

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

void NoteSequenceEngine::setup(const TrackSetup &trackSetup) {
    reset();
}

void NoteSequenceEngine::reset() {
    _currentStep = -1;
    _direction = 1;
    _gate = false;
    _gateOutput = false;
    _gateQueue.clear();
}

void NoteSequenceEngine::tick(uint32_t tick) {
    ASSERT(_sequence != nullptr, "invalid sequence");
    const auto &sequence = *_sequence;

    const uint32_t divisor = (CONFIG_PPQN / 4);

    if (tick % divisor == 0) {
        advance();
        const auto &step = sequence.step(_currentStep);
        if (evalStepGate(step)) {
            _gateQueue.push({ tick, true });
            _gateQueue.push({ tick + (divisor * evalStepLength(step)) / NoteSequence::Length::Range, false });
        }

        const auto &scale = Scale::scale(sequence.scale());
        _cvOutput = evalStepNote(step, scale);
    }

    while (!_gateQueue.empty() && tick >= _gateQueue.front().tick) {
        _gate = _gateQueue.front().gate;
        _gateOutput = !_mute && _gate;
        _gateQueue.pop();
    }
}

void NoteSequenceEngine::setSequence(const Sequence &sequence) {
    _sequence = &sequence.noteSequence();
}

void NoteSequenceEngine::setMute(bool mute) {
    _mute = mute;
}

void NoteSequenceEngine::advance() {
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
        case NoteSequence::PlayMode::Forward:
        case NoteSequence::PlayMode::PingPong:
        case NoteSequence::PlayMode::Pendulum:
            _currentStep = firstStep;
            break;
        case NoteSequence::PlayMode::Backward:
            _currentStep = lastStep;
            break;
        case NoteSequence::PlayMode::Random:
            _currentStep = randomStep();
            break;
        case NoteSequence::PlayMode::Last:
            break;
        }
    } else {
        // advance step
        switch (playMode) {
        case NoteSequence::PlayMode::Forward:
            _currentStep = _currentStep >= lastStep ? firstStep : _currentStep + 1;
            break;
        case NoteSequence::PlayMode::Backward:
            _currentStep = _currentStep <= firstStep ? lastStep : _currentStep - 1;
            break;
        case NoteSequence::PlayMode::PingPong:
        case NoteSequence::PlayMode::Pendulum:
            if (_direction > 0 && _currentStep >= lastStep) {
                _direction = -1;
            } else if (_direction < 0 && _currentStep <= firstStep) {
                _direction = 1;
            } else {
                if (playMode == NoteSequence::PlayMode::Pendulum) {
                    _currentStep += _direction;
                }
            }
            if (playMode == NoteSequence::PlayMode::PingPong) {
                _currentStep += _direction;
            }
            break;
        case NoteSequence::PlayMode::Random:
            _currentStep = randomStep();
            break;
        case NoteSequence::PlayMode::Last:
            break;
        }
    }
}
