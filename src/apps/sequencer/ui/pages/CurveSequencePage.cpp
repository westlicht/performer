#include "CurveSequencePage.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "engine/Curve.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Shape   = 0,
    Min     = 1,
    Max     = 2,
};

static const char *functionNames[] = { "SHAPE", "MIN", "MAX", nullptr, nullptr };


static void drawCurve(Canvas &canvas, int x, int y, int w, int h, float &lastY, const Curve::Function function, float min, float max) {
    const int Step = 2;

    auto eval = [=] (float x) {
        return (1.f - (function(x) * (max - min) + min)) * h;
    };

    float fy0 = y + eval(0.f);

    if (lastY != 0.f) {
        canvas.line(x, lastY, x, fy0);
    }

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
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    const auto &sequenceEngine = _engine.selectedTrackEngine().curveSequenceEngine();
    const auto &sequence = _project.selectedSequence().curveSequence();
    bool isActiveSequence = sequenceEngine.isActiveSequence(sequence);

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
    float lastY = 0.f;
    for (int stepIndex = 0; stepIndex < 16; ++stepIndex) {
        const auto &step = sequence.step(stepIndex);

        int x = stepIndex * stepWidth;

        bool selected = _stepSelection[stepIndex];
        float min = step.minNormalized();
        float max = step.maxNormalized();
        const auto function = Curve::function(Curve::Type(std::min(Curve::Last - 1, step.shape())));

        if (selected) {
            canvas.setColor(0x3);
            canvas.fillRect(x + 2, 16 - 2, stepWidth - 3, 32 + 4);
            canvas.setColor(0xf);
        } else {
            canvas.setBlendMode(BlendMode::Add);
        }

        drawCurve(canvas, x, 16, stepWidth, 32, lastY, function, min, max);
    }

    // draw cursor
    if (isActiveSequence) {
        canvas.setColor(0xf);
        int x = (sequenceEngine.currentStep() + sequenceEngine.currentStepFraction()) * stepWidth;
        canvas.vline(x, 16, 32);
    }
}

void CurveSequencePage::updateLeds(Leds &leds) {
}

void CurveSequencePage::keyDown(KeyEvent &event) {
    _stepSelection.keyDown(event, 0);
}

void CurveSequencePage::keyUp(KeyEvent &event) {
    _stepSelection.keyUp(event, 0);
}

void CurveSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &sequence = _project.selectedSequence().curveSequence();

    if (key.pageModifier()) {
        return;
    }

    _stepSelection.keyPress(event, 0);

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Shape:
            _mode = Mode::Shape;
            break;
        case Function::Min:
            _mode = Mode::Min;
            break;
        case Function::Max:
            _mode = Mode::Max;
            break;
        }
        event.consume();
    }

    if (key.shiftModifier() && key.is(Key::Left)) {
        sequence.shift(-1);
        event.consume();
    }
    if (key.shiftModifier() && key.is(Key::Right)) {
        sequence.shift(1);
        event.consume();
    }
}

void CurveSequencePage::encoder(EncoderEvent &event) {
    auto &sequence = _project.selectedSequence().curveSequence();

    for (size_t i = 0; i < sequence.steps().size(); ++i) {
        if (_stepSelection[i] || _keyState[Key::Shift]) {
            auto &step = sequence.step(i);
            switch (_mode) {
            case Mode::Shape:
                step.setShape(CurveSequence::Shape::clamp(step.shape() + event.value()));
                break;
            case Mode::Min:
                step.setMin(CurveSequence::Min::clamp(step.min() + event.value() * (event.pressed() ? 1 : 8)));
                break;
            case Mode::Max:
                step.setMax(CurveSequence::Max::clamp(step.max() + event.value() * (event.pressed() ? 1 : 8)));
                break;
            default:
                break;
            }
        }
    }

    event.consume();
}
