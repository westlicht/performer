#include "CurveSequencePage.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "engine/Curve.h"

#include "core/utils/StringBuilder.h"

static void drawCurve(Canvas &canvas, int x, int y, int w, int h, const Curve::Function function, float min, float max) {
    const int Step = 1;

    auto eval = [=] (float x) {
        return (1.f - (function(x) * (max - min) + min)) * h;
    };

    float fy0 = y + eval(0.f);
    for (int i = 0; i < w; i += Step) {
        float fy1 = y + eval((float(i) + Step) / w);
        canvas.line(x + i, fy0, x + i + Step, fy1);
        fy0 = fy1;
    }
}

CurveSequencePage::CurveSequencePage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void CurveSequencePage::enter() {
}

void CurveSequencePage::exit() {
}

void CurveSequencePage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, modeName(_mode));

    const char *functionNames[] = { "SHAPE", "MIN", "MAX", nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = track.sequence();
    const auto &curveSequence = sequence.curveSequence();

    canvas.setBlendMode(BlendMode::Add);

    const int stepWidth = 32;

    for (int stepIndex = 0; stepIndex < 16; ++stepIndex) {
        const auto &step = curveSequence.step(stepIndex);

        int x = stepIndex * stepWidth;

        bool selected = _selectedSteps[stepIndex] || _keyState[Key::Shift];
        float min = float(step.min()) / CurveSequence::Min::Max;
        float max = float(step.max()) / CurveSequence::Max::Max;
        const auto function = Curve::function(Curve::Type(std::min(Curve::Last - 1, step.shape())));

        canvas.setColor(selected ? 0xf : 0x7);

        drawCurve(canvas, x, 16, stepWidth, 32, function, min, max);
    }
}

void CurveSequencePage::updateLeds(Leds &leds) {
    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = track.sequence();

    LedPainter::drawSequenceGateAndCurrentStep(leds, sequence, track.currentStep());
}

void CurveSequencePage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isStep()) {
        _selectedSteps.add(key.step());
        event.consume();
    }

    if (key.isFunction()) {
        switch (key.function()) {
        case 0: _mode = Mode::Shape; break;
        case 1: _mode = Mode::Min; break;
        case 2: _mode = Mode::Max; break;
        default: break;
        }
        event.consume();
    }
}

void CurveSequencePage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isStep()) {
        _selectedSteps.remove(key.step());
        event.consume();
    }

    if (key.isFunction()) {
        event.consume();
    }
}

void CurveSequencePage::encoder(EncoderEvent &event) {
    auto &curveSequence = _project.selectedSequence().curveSequence();

    for (size_t i = 0; i < curveSequence.steps().size(); ++i) {
        if (_selectedSteps[i] || _keyState[Key::Shift]) {
            auto &step = curveSequence.step(i);
            switch (_mode) {
            case Mode::Shape:
                step.setShape(CurveSequence::Shape::clamp(step.shape() + event.value()));
                break;
            case Mode::Min:
                step.setMin(CurveSequence::Min::clamp(step.min() + event.value() * (event.pressed() ? 8 : 1)));
                break;
            case Mode::Max:
                step.setMax(CurveSequence::Max::clamp(step.max() + event.value() * (event.pressed() ? 8 : 1)));
                break;
            default:
                break;
            }
        }
    }

    event.consume();
}
