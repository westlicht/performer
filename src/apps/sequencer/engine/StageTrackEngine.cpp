#include "StageTrackEngine.h"

#include "Engine.h"
#include "Groove.h"
#include "SequenceUtils.h"

#include "core/Debug.h"
#include "core/utils/Random.h"
#include "core/math/Math.h"

#include "model/Scale.h"

static Random rng;

// evaluate note voltage
static float evalStepNote(const StageSequence::Step &step, int channel, const Scale &scale, int rootNote, int octave, int transpose) {
    int note = step.note(channel) + (scale.isChromatic() ? rootNote : 0) + octave * scale.notesPerOctave() + transpose;
    return scale.noteToVolts(note);
}

void StageTrackEngine::reset() {
    _sequenceState.reset();
    _currentStep = -1;
    _startBeat = 0;

    _activity = false;

    for (auto &channel : _channels) {
        channel.gateOutput = false;
        channel.cvOutput = 0.f;
        channel.cvOutputTarget = 0.f;
        channel.slideActive = false;
        channel.gateQueue.clear();
        channel.cvQueue.clear();
    }

    changePattern();
}

void StageTrackEngine::restart() {
    _sequenceState.reset();
    _currentStep = -1;
}

void StageTrackEngine::tick(uint32_t tick) {
    ASSERT(_sequence != nullptr, "invalid sequence");
    const auto &sequence = *_sequence;
    // const auto &scale = sequence.selectedScale(_model.project().scale());

    uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
    uint32_t measureDivisor = (sequence.resetMeasure() * CONFIG_PPQN * 4);
    uint32_t relativeTick = measureDivisor == 0 ? tick : tick % measureDivisor;

    // handle reset measure
    if (relativeTick == 0) {
        reset();
    }

    if (relativeTick % divisor == 0) {
        beat(tick, relativeTick, divisor);
    }

    for (auto &channel : _channels) {
        while (!channel.gateQueue.empty() && tick >= channel.gateQueue.front().tick) {
            _activity = channel.gateQueue.front().gate;
            channel.gateOutput = (!mute() || fill()) && _activity;
            channel.gateQueue.pop();

            // _engine.midiOutputEngine().sendGate(_track.trackIndex(), _gateOutput);
        }

        while (!channel.cvQueue.empty() && tick >= channel.cvQueue.front().tick) {
            channel.cvOutputTarget = channel.cvQueue.front().cv;
            // channel.slideActive = channel.cvQueue.front().slide;
            channel.cvQueue.pop();

            // _engine.midiOutputEngine().sendCv(_track.trackIndex(), _cvOutputTarget);
        }
    }
}

void StageTrackEngine::update(float dt) {
#if 0
    bool running = _engine.state().running();

    const auto &sequence = *_sequence;
    const auto &scale = sequence.selectedScale(_model.project().scale());
    int rootNote = sequence.selectedRootNote(_model.project().rootNote());

    // override due to monitoring or recording
    if (!running) {
        if (_monitorStepIndex >= 0) {
            // step monitoring (first priority)
            const auto &step = sequence.step(_monitorStepIndex);
            int octave = _stageTrack.octave();
            int transpose = _stageTrack.transpose();
            _cvOutputTarget = evalStepNote(step, scale, rootNote, octave, transpose, false);
            _activity = _gateOutput = true;
            _monitorOverrideActive = true;
        } else {
            if (_monitorOverrideActive) {
                _activity = _gateOutput = false;
                _monitorOverrideActive = false;
            }
        }
    }
#endif

    for (auto &channel : _channels) {
        if (channel.slideActive && _stageTrack.slideTime() > 0) {
            channel.cvOutput += (channel.cvOutputTarget - channel.cvOutput) * std::min(1.f, dt * (200 - 2 * _stageTrack.slideTime()));
        } else {
            channel.cvOutput = channel.cvOutputTarget;
        }
    }
}

void StageTrackEngine::changePattern() {
    _sequence = &_stageTrack.sequence(pattern());
}

void StageTrackEngine::monitorMidi(uint32_t tick, const MidiMessage &message) {
}

void StageTrackEngine::setMonitorStep(int index) {
    _monitorStepIndex = (index >= 0 && index < CONFIG_STEP_COUNT) ? index : -1;
}

void StageTrackEngine::beat(uint32_t tick, uint32_t relativeTick, uint32_t divisor) {
    const auto &sequence = *_sequence;
    uint32_t beat = relativeTick / divisor - _startBeat;

    // advance sequence
    if (_currentStep >= 0 && beat >= uint32_t(sequence.step(_currentStep).length())) {
        _startBeat += beat;
        beat = 0;
    }
    if (beat == 0) {
        _sequenceState.advanceFree(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
        _currentStep = _sequenceState.step();
        // triggerStep(tick, divisor);
    }

    triggerBeat(beat, tick, divisor);

#if 0
    const auto &step = sequence.step(_currentStep);

    // trigger step
    uint32_t relativeBeat = beat % step.length();
    uint32_t gate = step.gate();
    uint8_t gatePattern = step.gatePattern();
    if (gate > 0 && relativeBeat < gate) {
        if (gatePattern) {
            if (gatePattern & (1 << relativeBeat)) {
                _gateQueue.push({ applySwing(tick), true });
                _gateQueue.push({ applySwing(tick + divisor / 2), false });
            }
        } else {
            if (relativeBeat == 0) {
                _gateQueue.push({ applySwing(tick), true });
                _gateQueue.push({ applySwing(tick + step.gate() * divisor), false });
            }
        }
    }
#endif
}

void StageTrackEngine::triggerBeat(uint32_t beat, uint32_t tick, uint32_t divisor) {
    int octave = _stageTrack.octave();
    int transpose = _stageTrack.transpose();
    int rotate = _stageTrack.rotate();
    // bool useFillGates = fill() && _stageTrack.fillMode() == Types::FillMode::Gates;
    // bool useFillSequence = fill() && _stageTrack.fillMode() == Types::FillMode::NextPattern;

    const auto &sequence = *_sequence;
    // const auto &evalSequence = useFillSequence ? *_fillSequence : *_sequence;
    const auto &evalSequence = *_sequence;
    _currentStep = SequenceUtils::rotateStep(_sequenceState.step(), sequence.firstStep(), sequence.lastStep(), rotate);
    const auto &step = evalSequence.step(_currentStep);
    int length = step.length();
    uint32_t stepStartTick = tick - beat * divisor;

    for (int channelIndex = 0; channelIndex < 2; ++channelIndex) {
        auto &channel = _channels[channelIndex];

        int gateCount = step.gateCount(channelIndex);
        uint32_t gateLengthTicks = gateCount > 0 ? (length * divisor) / (gateCount * 2) : 0;
        for (int gateIndex = 0; gateIndex < gateCount; ++gateIndex) {
            uint32_t gateStartTick = stepStartTick + (length * divisor * gateIndex) / gateCount;
            if (gateStartTick < stepStartTick + beat * divisor) {
                continue;
            }
            if (gateStartTick >= stepStartTick + (beat + 1) * divisor) {
                break;
            }
            channel.gateQueue.push({ Groove::applySwing(gateStartTick, swing()), true });
            channel.gateQueue.push({ Groove::applySwing(gateStartTick + gateLengthTicks, swing()), false });
        }

        if (beat == 0) {
            const auto &scale = evalSequence.selectedScale(_model.project().scale());
            int rootNote = evalSequence.selectedRootNote(_model.project().rootNote());
            channel.cvQueue.push({ Groove::applySwing(tick, swing()), evalStepNote(step, channelIndex, scale, rootNote, octave, transpose), false });

        }
    }

}

void StageTrackEngine::triggerStep(uint32_t tick, uint32_t divisor) {
#if 0
    int octave = _stageTrack.octave();
    int transpose = _stageTrack.transpose();
    int rotate = _stageTrack.rotate();
    bool useFillGates = fill() && _stageTrack.fillMode() == Types::FillMode::Gates;
    bool useFillSequence = fill() && _stageTrack.fillMode() == Types::FillMode::NextPattern;

    const auto &sequence = *_sequence;
    const auto &evalSequence = useFillSequence ? *_fillSequence : *_sequence;
    _currentStep = SequenceUtils::rotateStep(_sequenceState.step(), sequence.firstStep(), sequence.lastStep(), rotate);
    const auto &step = evalSequence.step(_currentStep);

    if (evalStepGate(step, _stageTrack.stepGateProbabilityBias()) || useFillGates) {
        uint32_t stepLength = (divisor * evalStepLength(step, _stageTrack.stepLengthBias())) / NoteSequence::Length::Range;
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

        const auto &scale = evalSequence.selectedScale(_model.project().scale());
        int rootNote = evalSequence.selectedRootNote(_model.project().rootNote());
        _cvQueue.push({ applySwing(tick), evalStepNote(step, scale, rootNote, octave, transpose), step.slide() });
    }
#endif
}
