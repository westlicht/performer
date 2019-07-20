#include "HarmonyTrackEngine.h"

#include "Engine.h"
#include "Groove.h"
#include "SequenceUtils.h"

#include "core/Debug.h"
#include "core/utils/Random.h"
#include "core/math/Math.h"

#include "model/Scale.h"

static Random rng;

void HarmonyTrackEngine::reset() {
    _sequenceState.reset();
    _currentStep = -1;
    _startBeat = 0;

    _activity = false;

    changePattern();
}

void HarmonyTrackEngine::tick(uint32_t tick) {
    ASSERT(_sequence != nullptr, "invalid sequence");
    const auto &sequence = *_sequence;

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
}

void HarmonyTrackEngine::update(float dt) {
}

void HarmonyTrackEngine::changePattern() {
    _sequence = &_harmonyTrack.sequence(pattern());
}

void HarmonyTrackEngine::monitorMidi(uint32_t tick, const MidiMessage &message) {
}

void HarmonyTrackEngine::setMonitorStep(int index) {
}

void HarmonyTrackEngine::beat(uint32_t tick, uint32_t relativeTick, uint32_t divisor) {
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
    }

    const auto &step = sequence.step(_currentStep);

    // set chord scale
    if (beat == 0) {
        Scale::chordScale().setRoot(step.chordRoot());
        Scale::chordScale().setQuality(step.chordQuality());
        Scale::chordScale().setVoicing(step.chordVoicing());
    }
}
