#include "CurveSequenceEditPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "model/Curve.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Duplicate,
    Generate,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUPL" },
    { "GEN" },
};

enum class Function {
    Shape   = 0,
    Min     = 1,
    Max     = 2,
};

static const char *functionNames[] = { "SHAPE", "MIN", "MAX", nullptr, nullptr };


static void drawCurve(Canvas &canvas, int x, int y, int w, int h, float &lastY, const Curve::Function function, float min, float max) {
    const int Step = 1;

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

CurveSequenceEditPage::CurveSequenceEditPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void CurveSequenceEditPage::enter() {
    resetKeyState();
}

void CurveSequenceEditPage::exit() {
}

void CurveSequenceEditPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE EDIT");
    WindowPainter::drawActiveFunction(canvas, CurveSequence::layerName(layer()));
    WindowPainter::drawFooter(canvas, functionNames, keyState());

    const auto &trackEngine = _engine.selectedTrackEngine().as<CurveTrackEngine>();
    const auto &sequence = _project.selectedCurveSequence();
    bool isActiveSequence = trackEngine.isActiveSequence(sequence);

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
    SequencePainter::drawLoopEnd(canvas, (sequence.lastStep() - stepOffset) * stepWidth + 1, loopY, stepWidth - 2);

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
        int x = ((trackEngine.currentStep() - stepOffset) + trackEngine.currentStepFraction()) * stepWidth;
        canvas.vline(x, curveY, curveHeight);
    }
}

void CurveSequenceEditPage::updateLeds(Leds &leds) {
    LedPainter::drawSelectedSequencePage(leds, _page);
}

void CurveSequenceEditPage::keyDown(KeyEvent &event) {
    _stepSelection.keyDown(event, stepOffset());
}

void CurveSequenceEditPage::keyUp(KeyEvent &event) {
    _stepSelection.keyUp(event, stepOffset());
}

void CurveSequenceEditPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &sequence = _project.selectedCurveSequence();

    if (key.isContextMenu()) {
        contextShow();
        event.consume();
        return;
    }

    if (key.isQuickEdit()) {
        quickEdit(key.quickEdit());
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
            setLayer(Layer::Shape);
            break;
        case Function::Min:
            setLayer(Layer::Min);
            break;
        case Function::Max:
            setLayer(Layer::Max);
            break;
        }
        event.consume();
    }

    if (key.isLeft()) {
        if (key.shiftModifier()) {
            sequence.shiftSteps(-1);
        } else {
            _page = std::max(0, _page - 1);
        }
        event.consume();
    }
    if (key.isRight()) {
        if (key.shiftModifier()) {
            sequence.shiftSteps(1);
        } else {
            _page = std::min(3, _page + 1);
        }
        event.consume();
    }
}

void CurveSequenceEditPage::encoder(EncoderEvent &event) {
    auto &sequence = _project.selectedCurveSequence();

    if (!_stepSelection.any()) {
        return;
    }

    const auto &firstStep = sequence.step(_stepSelection.first());

    for (size_t stepIndex = 0; stepIndex < sequence.steps().size(); ++stepIndex) {
        if (_stepSelection[stepIndex]) {
            auto &step = sequence.step(stepIndex);
            bool setToFirst = int(stepIndex) != _stepSelection.first() && globalKeyState()[Key::Shift];
            switch (layer()) {
            case Layer::Shape:
                step.setShape(
                    setToFirst ? firstStep.shape() :
                    step.shape() + event.value()
                );
                break;
            case Layer::Min:
                step.setMin(
                    setToFirst ? firstStep.min() :
                    step.min() + event.value() * (event.pressed() ? 1 : 8)
                );
                break;
            case Layer::Max:
                step.setMax(
                    setToFirst ? firstStep.max() :
                    step.max() + event.value() * (event.pressed() ? 1 : 8)
                );
                break;
            default:
                break;
            }
        }
    }

    event.consume();
}

void CurveSequenceEditPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void CurveSequenceEditPage::contextAction(int index) {
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
    case ContextAction::Duplicate:
        duplicateSequence();
        break;
    case ContextAction::Generate:
        generateSequence();
        break;
    case ContextAction::Last:
        break;
    }
}

bool CurveSequenceEditPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteCurveSequenceSteps();
    default:
        return true;
    }
}

void CurveSequenceEditPage::initSequence() {
    _project.selectedCurveSequence().clearSteps();
    showMessage("STEPS INITIALIZED");
}

void CurveSequenceEditPage::copySequence() {
    _model.clipBoard().copyCurveSequenceSteps(_project.selectedCurveSequence(), _stepSelection.selected());
    showMessage("STEPS COPIED");
}

void CurveSequenceEditPage::pasteSequence() {
    _model.clipBoard().pasteCurveSequenceSteps(_project.selectedCurveSequence(), _stepSelection.selected());
    showMessage("STEPS PASTED");
}

void CurveSequenceEditPage::duplicateSequence() {
    _project.selectedCurveSequence().duplicateSteps();
    showMessage("STEPS DUPLICATED");
}

void CurveSequenceEditPage::generateSequence() {
    _manager.pages().generatorSelect.show([this] (bool success, Generator::Mode mode) {
        if (success) {
            auto builder = _builderContainer.create<CurveSequenceBuilder>(_project.selectedCurveSequence(), layer());
            auto generator = Generator::create(mode, *builder);
            _manager.pages().generator.show(generator);
        }
    });
}

void CurveSequenceEditPage::quickEdit(int index) {
    static const CurveSequenceListModel::Item itemMap[8] = {
        CurveSequenceListModel::Item::Range,
        CurveSequenceListModel::Item::Last,
        CurveSequenceListModel::Item::Divisor,
        CurveSequenceListModel::Item::ResetMeasure,
        CurveSequenceListModel::Item::RunMode,
        CurveSequenceListModel::Item::FirstStep,
        CurveSequenceListModel::Item::LastStep,
        CurveSequenceListModel::Item::Last
    };

    _listModel.setSequence(&_project.selectedCurveSequence());
    if (itemMap[index] != CurveSequenceListModel::Item::Last) {
        _manager.pages().quickEdit.show(_listModel, int(itemMap[index]));
    }
}
