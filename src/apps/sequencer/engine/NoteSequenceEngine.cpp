#include "NoteSequenceEngine.h"

#include "Scale.h"
#include "Groove.h"

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

// evaluate step retrigger count
static int evalStepRetrigger(const NoteSequence::Step &step) {
    return
        (step.retriggerProbability() == NoteSequence::RetriggerProbability::Max ||
         int(rng.nextRange(NoteSequence::RetriggerProbability::Range)) <= step.retriggerProbability()) ?
         step.retrigger() + 1 : 1;
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
    _trackSetup = &trackSetup;
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

    uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    uint32_t measureDivisor = (sequence.resetMeasure() * CONFIG_PPQN * 4);

    if (measureDivisor != 0 && tick % measureDivisor == 0) {
        reset();
    }

    if (tick % divisor == 0) {
        advance(tick / divisor);
        const auto &step = sequence.step(_currentStep);
        if (evalStepGate(step) || _fill) {
            uint32_t stepLength = (divisor * evalStepLength(step)) / NoteSequence::Length::Range;
            int stepRetrigger = evalStepRetrigger(step);
            if (stepRetrigger > 1) {
                uint32_t retriggerLength = divisor / stepRetrigger;
                uint32_t stepOffset = 0;
                while (stepRetrigger-- > 0 && stepOffset <= stepLength) {
                    _gateQueue.push({ applySwing(tick + stepOffset), true });
                    _gateQueue.push({ applySwing(tick + stepOffset + retriggerLength / 2), false });
                    stepOffset += retriggerLength;
                }
            } else {
                _gateQueue.push({ applySwing(tick), true });
                _gateQueue.push({ applySwing(tick + stepLength), false });
            }

            const auto &scale = Scale::get(sequence.scale());
            _cvQueue.push({ applySwing(tick), evalStepNote(step, scale) });
        }
    }

    while (!_gateQueue.empty() && tick >= _gateQueue.front().tick) {
        _gate = _gateQueue.front().gate;
        _gateOutput = !_mute && _gate;
        _gateQueue.pop();
    }

    while (!_cvQueue.empty() && tick >= _cvQueue.front().tick) {
        _cvOutput = _cvQueue.front().cv;
        _cvQueue.pop();
    }
}

void NoteSequenceEngine::setSequence(const Sequence &sequence) {
    _sequence = &sequence.noteSequence();
}

void NoteSequenceEngine::setSwing(int swing) {
    _swing = swing;
}

void NoteSequenceEngine::setMute(bool mute) {
    _mute = mute;
}

void NoteSequenceEngine::setFill(bool fill) {
    _fill = fill;
}

void NoteSequenceEngine::advance(int step) {
    switch (_trackSetup->playMode()) {
    case TrackSetup::PlayMode::Free:
        advanceFree();
        break;
    case TrackSetup::PlayMode::Aligned:
        advanceAligned(step);
        break;
    case TrackSetup::PlayMode::Last:
        break;
    }
}

void NoteSequenceEngine::advanceFree() {
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

void NoteSequenceEngine::advanceAligned(int step) {
    const auto &sequence = *_sequence;

    auto playMode = sequence.playMode();
    int firstStep = sequence.firstStep();
    int lastStep = sequence.lastStep();
    int stepCount = lastStep - firstStep + 1;
    ASSERT(firstStep <= lastStep, "invalid first/last step");

    switch (playMode) {
    case NoteSequence::PlayMode::Forward:
        _currentStep = firstStep + step % stepCount;
        break;
    case NoteSequence::PlayMode::PingPong:
        step %= 2 * stepCount - 2;
        _currentStep = (step < stepCount) ? (firstStep + step) : (lastStep - (step - stepCount) - 1);
        break;
    case NoteSequence::PlayMode::Pendulum:
        step %= 2 * stepCount;
        _currentStep = (step < stepCount) ? (firstStep + step) : (lastStep - (step - stepCount));
        break;
    case NoteSequence::PlayMode::Backward:
        _currentStep = lastStep - step % stepCount;
        break;
    case NoteSequence::PlayMode::Random:
        _currentStep = firstStep + rng.nextRange(stepCount);
        break;
    case NoteSequence::PlayMode::Last:
        break;
    }
}

uint32_t NoteSequenceEngine::applySwing(uint32_t tick) {
    return Groove::swing(tick, CONFIG_PPQN / 4, _swing);
}
