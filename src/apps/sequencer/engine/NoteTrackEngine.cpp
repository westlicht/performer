#include "NoteTrackEngine.h"

#include "Scale.h"
#include "Groove.h"

#include "core/Debug.h"
#include "core/utils/Random.h"
#include "core/math/Math.h"

#include "engine/SequenceUtils.h"

static Random rng;

// evaluate if step gate is active
static bool evalStepGate(const NoteSequence::Step &step, int gateProbabilityOffset) {
    int gateProbability = clamp(step.gateProbability() + gateProbabilityOffset, -1, NoteSequence::GateProbability::Max);
    return step.gate() &&
        (gateProbability == NoteSequence::GateProbability::Max ||
         int(rng.nextRange(NoteSequence::GateProbability::Range)) <= gateProbability);
}

// evaluate step retrigger count
static int evalStepRetrigger(const NoteSequence::Step &step) {
    return
        (step.retriggerProbability() == NoteSequence::RetriggerProbability::Max ||
         int(rng.nextRange(NoteSequence::RetriggerProbability::Range)) <= step.retriggerProbability()) ?
         step.retrigger() + 1 : 1;
}

// evaluate step length
static int evalStepLength(const NoteSequence::Step &step, int lengthOffset) {
    int length = NoteSequence::Length::clamp(step.length() + lengthOffset) + 1;
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

static float evalStepNote(const NoteSequence::Step &step, const Scale &scale, int octave, int transpose) {
    return scale.noteVolts(step.note() + octave * scale.octave() + transpose);
}


void NoteTrackEngine::reset() {
    _sequenceState.reset();
    _currentStep = -1;
    _gate = false;
    _idleOutput = false;
    _gateOutput = false;
    _cvOutput = 0.f;
    _cvOutputTarget = 0.f;
    _slideActive = false;
    _gateQueue.clear();
    _cvQueue.clear();
    changePattern();
}

void NoteTrackEngine::tick(uint32_t tick) {
    ASSERT(_sequence != nullptr, "invalid sequence");
    const auto &sequence = *_sequence;
    const auto *linkData = _linkedTrackEngine ? _linkedTrackEngine->linkData() : nullptr;

    if (linkData) {
        _linkData = *linkData;
        _sequenceState = *linkData->sequenceState;

        if (linkData->relativeTick == 0) {
            triggerStep(tick, linkData->divisor);
        }
    } else {
        uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
        uint32_t measureDivisor = (sequence.resetMeasure() * CONFIG_PPQN * 4);
        uint32_t relativeTick = measureDivisor == 0 ? tick : tick % measureDivisor;

        // handle reset measure
        if (relativeTick == 0) {
            reset();
        }

        relativeTick %= divisor;

        // advance sequence
        if (relativeTick == 0) {
            switch (_noteTrack.playMode()) {
            case Types::PlayMode::Free:
                _sequenceState.advanceFree(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
                break;
            case Types::PlayMode::Aligned:
                _sequenceState.advanceAligned(tick / divisor, sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
                break;
            case Types::PlayMode::Last:
                break;
            }

            triggerStep(tick, divisor);
        }

        _linkData.divisor = divisor;
        _linkData.relativeTick = relativeTick;
        _linkData.sequenceState = &_sequenceState;
    }

    while (!_gateQueue.empty() && tick >= _gateQueue.front().tick) {
        _gate = _gateQueue.front().gate;
        _gateOutput = !_mute && _gate;
        _gateQueue.pop();
    }

    while (!_cvQueue.empty() && tick >= _cvQueue.front().tick) {
        _cvOutputTarget = _cvQueue.front().cv;
        _slideActive = _cvQueue.front().slide;
        _cvQueue.pop();
    }
}

void NoteTrackEngine::update(float dt) {
    if (_slideActive && _noteTrack.slideTime() > 0) {
        _cvOutput += (_cvOutputTarget - _cvOutput) * std::min(1.f, dt * (200 - 2 * _noteTrack.slideTime()));
    } else {
        _cvOutput = _cvOutputTarget;
    }
}

void NoteTrackEngine::changePattern() {
    _sequence = &_noteTrack.sequence(_pattern);
    _fillSequence = &_noteTrack.sequence(std::min(_pattern + 1, CONFIG_PATTERN_COUNT - 1));
}

void NoteTrackEngine::clearIdleOutput() {
    _idleOutput = false;
}

void NoteTrackEngine::setIdleStep(int index) {
    _idleOutput = true;
    const auto &sequence = *_sequence;
    const auto &step = sequence.step(index);
    const auto &scale = Scale::get(sequence.scale());
    int octave = _noteTrack.octave();
    int transpose = _noteTrack.transpose();
    _idleCvOutput = evalStepNote(step, scale, octave, transpose);
}

void NoteTrackEngine::setIdleGate(bool gate) {
    _idleGateOutput = gate;
}

void NoteTrackEngine::triggerStep(uint32_t tick, uint32_t divisor) {
    int octave = _noteTrack.octave();
    int transpose = _noteTrack.transpose();
    int rotate = _noteTrack.rotate();
    bool useFillGates = _fill && _noteTrack.fillMode() == Types::FillMode::Gates;
    bool useFillSequence = _fill && _noteTrack.fillMode() == Types::FillMode::NextPattern;

    const auto &sequence = *_sequence;
    const auto &evalSequence = useFillSequence ? *_fillSequence : *_sequence;
    _currentStep = SequenceUtils::rotateStep(_sequenceState.step(), sequence.firstStep(), sequence.lastStep(), rotate);
    const auto &step = evalSequence.step(_currentStep);

    if (evalStepGate(step, _noteTrack.stepProbabilityBias()) || useFillGates) {
        uint32_t stepLength = (divisor * evalStepLength(step, _noteTrack.stepLengthBias())) / NoteSequence::Length::Range;
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

        const auto &scale = Scale::get(evalSequence.scale());
        _cvQueue.push({ applySwing(tick), evalStepNote(step, scale, octave, transpose), step.slide() });
    }
}

uint32_t NoteTrackEngine::applySwing(uint32_t tick) {
    return Groove::swing(tick, CONFIG_PPQN / 4, _swing);
}
