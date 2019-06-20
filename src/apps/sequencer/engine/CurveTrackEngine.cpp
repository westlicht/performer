#include "CurveTrackEngine.h"

#include "Engine.h"
#include "SequenceUtils.h"

#include "core/Debug.h"
#include "core/utils/Random.h"
#include "core/math/Math.h"

#include "model/Curve.h"
#include "model/Types.h"

static Random rng;

static float evalStepShape(const CurveSequence::Step &step, float fraction) {
    auto function = Curve::function(Curve::Type(step.shape()));
    float value = function(fraction);
    float min = float(step.min()) / CurveSequence::Min::Max;
    float max = float(step.max()) / CurveSequence::Max::Max;
    return min + value * (max - min);
}

void CurveTrackEngine::reset() {
    _sequenceState.reset();
    _currentStep = -1;
    _currentStepFraction = 0.f;

    _recorder.reset();

    changePattern();
}

void CurveTrackEngine::tick(uint32_t tick) {
    ASSERT(_sequence != nullptr, "invalid sequence");
    const auto &sequence = *_sequence;
    const auto *linkData = _linkedTrackEngine ? _linkedTrackEngine->linkData() : nullptr;

    if (linkData) {
        _linkData = *linkData;
        _sequenceState = *linkData->sequenceState;
        updateOutput(linkData->relativeTick, linkData->divisor);
    } else {
        uint32_t divisor = sequence.divisor() * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
        uint32_t measureDivisor = (sequence.resetMeasure() * CONFIG_PPQN * 4);
        uint32_t relativeTick = measureDivisor == 0 ? tick : tick % measureDivisor;

        // handle reset measure
        if (relativeTick == 0) {
            reset();
        }

        updateRecording(relativeTick, divisor);

        if (relativeTick % divisor == 0) {
            // advance sequence
            switch (_curveTrack.playMode()) {
            case Types::PlayMode::Aligned:
                _sequenceState.advanceAligned(relativeTick / divisor, sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
                break;
            case Types::PlayMode::Free:
                _sequenceState.advanceFree(sequence.runMode(), sequence.firstStep(), sequence.lastStep(), rng);
                break;
            case Types::PlayMode::Last:
                break;
            }
        }

        updateOutput(relativeTick, divisor);

        _linkData.divisor = divisor;
        _linkData.relativeTick = relativeTick;
        _linkData.sequenceState = &_sequenceState;
    }
}

void CurveTrackEngine::update(float dt) {
    // override due to recording
    if (isRecording()) {
        updateRecordValue();
        const auto &range = Types::voltageRangeInfo(_sequence->range());
        _cvOutputTarget = range.denormalize(_recordValue);
        _cvOutput = _cvOutputTarget;
    }

    if (_curveTrack.slideTime() > 0) {
        float factor = 1.f - 0.01f * _curveTrack.slideTime();
        factor = 500.f * factor * factor;
        _cvOutput += (_cvOutputTarget - _cvOutput) * std::min(1.f, dt * factor);
    } else {
        _cvOutput = _cvOutputTarget;
    }
}

void CurveTrackEngine::changePattern() {
    _sequence = &_curveTrack.sequence(pattern());
}

void CurveTrackEngine::updateOutput(uint32_t relativeTick, uint32_t divisor) {
    if (_sequenceState.step() < 0) {
        return;
    }

    int rotate = _curveTrack.rotate();

    const auto &sequence = *_sequence;
    _currentStep = SequenceUtils::rotateStep(_sequenceState.step(), sequence.firstStep(), sequence.lastStep(), rotate);
    const auto &step = sequence.step(_currentStep);

    _currentStepFraction = float(relativeTick % divisor) / divisor;

    float value = evalStepShape(step, _currentStepFraction);
    const auto &range = Types::voltageRangeInfo(sequence.range());
    value = range.denormalize(value);
    _cvOutputTarget = value;

    _engine.midiOutputEngine().sendCv(_track.trackIndex(), _cvOutputTarget);
}

bool CurveTrackEngine::isRecording() const {
    return
        _engine.state().recording() &&
        _model.project().curveCvInput() != Types::CurveCvInput::Off &&
        _model.project().selectedTrackIndex() == _track.trackIndex();
}

void CurveTrackEngine::updateRecordValue() {
    auto &sequence = *_sequence;
    const auto &range = Types::voltageRangeInfo(sequence.range());
    auto curveCvInput = _model.project().curveCvInput();

    switch (curveCvInput) {
    case Types::CurveCvInput::Cv1:
    case Types::CurveCvInput::Cv2:
    case Types::CurveCvInput::Cv3:
    case Types::CurveCvInput::Cv4:
        _recordValue = range.normalize(_engine.cvInput().channel(int(curveCvInput) - int(Types::CurveCvInput::Cv1)));
        break;
    default:
        _recordValue = 0.f;
        break;
    }
}

void CurveTrackEngine::updateRecording(uint32_t relativeTick, uint32_t divisor) {
    if (!isRecording()) {
        _recorder.reset();
        return;
    }

    updateRecordValue();

    if (_recorder.write(relativeTick, divisor, _recordValue) && _sequenceState.step() >= 0) {
        auto &sequence = *_sequence;
        int rotate = _curveTrack.rotate();
        auto &step = sequence.step(SequenceUtils::rotateStep(_sequenceState.step(), sequence.firstStep(), sequence.lastStep(), rotate));
        auto match = _recorder.matchCurve();
        step.setShape(match.type);
        step.setMinNormalized(match.min);
        step.setMaxNormalized(match.max);
    }
}
