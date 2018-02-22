#include "CurveSequencePage.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "engine/Curve.h"

#include "core/utils/StringBuilder.h"

static void drawCurve(Canvas &canvas, int x, int y, int w, int h, int &lastY, const Curve::Function function, float min, float max) {
    const int Step = 2;

    auto eval = [=] (float x) {
        return (1.f - (function(x) * (max - min) + min)) * h;
    };

    float fy0 = y + eval(0.f);

    canvas.vline(x, lastY, fy0 - lastY);

    for (int i = 0; i < w; i += Step) {
        float fy1 = y + eval((float(i) + Step) / w);
        canvas.line(x + i, fy0, x + i + Step, fy1);
        fy0 = fy1;
    }

    lastY = fy0;
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

    const auto &sequenceEngine = _engine.selectedTrackEngine().curveSequenceEngine();
    const auto &sequence = sequenceEngine.sequence();

    canvas.setBlendMode(BlendMode::Add);

    const int stepWidth = 32;
    const int stepCount = Width / stepWidth;

    // draw grid
    canvas.setColor(0x3);
    for (int stepIndex = 1; stepIndex < stepCount; ++stepIndex) {
        int x = stepIndex * stepWidth;
        canvas.vline(x, 16, 32);
    }

    // draw curve
    canvas.setColor(0xf);
    int lastY;
    for (int stepIndex = 0; stepIndex < 16; ++stepIndex) {
        const auto &step = sequence.step(stepIndex);

        int x = stepIndex * stepWidth;

        bool selected = _selectedSteps[stepIndex] || _keyState[Key::Shift];
        float min = step.minNormalized();
        float max = step.maxNormalized();
        const auto function = Curve::function(Curve::Type(std::min(Curve::Last - 1, step.shape())));

        if (selected) {
            canvas.setColor(0x3);
            canvas.fillRect(x, 16 - 2, stepWidth, 32 + 4);
            canvas.setColor(0xf);
            // canvas.setBlendMode(BlendMode::Sub);
        } else {
            canvas.setBlendMode(BlendMode::Add);
        }
        // canvas.setColor(selected ? 0xf : 0x7);

        drawCurve(canvas, x, 16, stepWidth, 32, lastY, function, min, max);
    }

    // draw cursor
    {
        canvas.setColor(0xf);
        int x = (sequenceEngine.currentStep() + sequenceEngine.currentStepFraction()) * stepWidth;
        canvas.vline(x, 16, 32);
    }
}

void CurveSequencePage::updateLeds(Leds &leds) {
    // const auto &sequence = _project.selectedTrackSequence();

    // LedPainter::drawSequenceGateAndCurrentStep(leds, sequence, trackEngine.currentStep());
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
