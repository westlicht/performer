#include "CurveSequencePage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "engine/Curve.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
};

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
    BasePage(manager, context),
    _contextMenu(
        manager.pages().contextMenu,
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    )
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

    const int stepWidth = Width / StepCount;
    const int stepOffset = this->stepOffset();

    const int loopY = 16;
    const int curveY = 24;
    const int curveHeight = 24;

    // draw loop points
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    SequencePainter::drawLoopStart(canvas, (sequence.firstStep() - stepOffset) * stepWidth + 1, loopY, stepWidth - 2);
    SequencePainter::drawLoopEnd(canvas, (sequence.lastStep()  - stepOffset)* stepWidth + 1, loopY, stepWidth - 2);

    // draw grid
    canvas.setColor(0x3);
    for (int stepIndex = 1; stepIndex < StepCount; ++stepIndex) {
        int x = stepIndex * stepWidth;
        canvas.vline(x, curveY, curveHeight);
    }

    // draw curve
    canvas.setColor(0xf);
    float lastY = 0.f;
    for (int i = 0; i < StepCount; ++i) {
        int stepIndex = stepOffset + i;
        const auto &step = sequence.step(stepIndex);

        int x = i * stepWidth;
        int y = 20;

        canvas.setBlendMode(BlendMode::Set);

        // loop
        if (stepIndex > sequence.firstStep() && stepIndex <= sequence.lastStep()) {
            canvas.setColor(0xf);
            canvas.point(x, loopY);
        }

        // step index
        {
            canvas.setColor(_stepSelection[stepIndex] ? 0xf : 0x7);
            FixedStringBuilder<8> str("%d", stepIndex + 1);
            canvas.drawText(x + (stepWidth - canvas.textWidth(str) + 1) / 2, y - 2, str);
        }

        // curve
        {
            float min = step.minNormalized();
            float max = step.maxNormalized();
            const auto function = Curve::function(Curve::Type(std::min(Curve::Last - 1, step.shape())));

            canvas.setColor(0xf);
            canvas.setBlendMode(BlendMode::Add);

            drawCurve(canvas, x, curveY, stepWidth, curveHeight, lastY, function, min, max);
        }
    }

    // draw cursor
    if (isActiveSequence) {
        canvas.setColor(0xf);
        int x = ((sequenceEngine.currentStep() - stepOffset) + sequenceEngine.currentStepFraction()) * stepWidth;
        canvas.vline(x, 16, 32);
    }
}

void CurveSequencePage::updateLeds(Leds &leds) {
    LedPainter::drawSelectedSequencePage(leds, _page);
}

void CurveSequencePage::keyDown(KeyEvent &event) {
    _stepSelection.keyDown(event, stepOffset());
}

void CurveSequencePage::keyUp(KeyEvent &event) {
    _stepSelection.keyUp(event, stepOffset());
}

void CurveSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &sequence = _project.selectedSequence().curveSequence();

    if (key.isContextMenu()) {
        _contextMenu.show();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    _stepSelection.keyPress(event, stepOffset());

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

    if (key.is(Key::Left)) {
        if (key.shiftModifier()) {
            sequence.shift(-1);
        } else {
            _page = std::max(0, _page - 1);
        }
        event.consume();
    }
    if (key.is(Key::Right)) {
        if (key.shiftModifier()) {
            sequence.shift(1);
        } else {
            _page = std::min(3, _page + 1);
        }
        event.consume();
    }
}

void CurveSequencePage::encoder(EncoderEvent &event) {
    auto &sequence = _project.selectedSequence().curveSequence();

    for (size_t i = 0; i < sequence.steps().size(); ++i) {
        if (_stepSelection[i]) {
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

void CurveSequencePage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initSequence();
        break;
    case ContextAction::Copy:
        copySequence();
        break;
    case ContextAction::Paste:
        pasteSequence();
        break;
    case ContextAction::Last:
        break;
    }
}

bool CurveSequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().sequenceBuffer().isCopied() && _model.clipBoard().sequenceBuffer().canPasteTo(_project.selectedSequence());
    default:
        return true;
    }
}

void CurveSequencePage::initSequence() {
    _project.selectedSequence().clear();
}

void CurveSequencePage::copySequence() {
    _model.clipBoard().sequenceBuffer().copyFrom(_project.selectedSequence());
}

void CurveSequencePage::pasteSequence() {
    _model.clipBoard().sequenceBuffer().pasteTo(_project.selectedSequence());
}
