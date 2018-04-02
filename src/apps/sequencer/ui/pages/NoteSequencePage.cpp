#include "NoteSequencePage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "engine/Scale.h"

#include "os/os.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Duplicate,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUPL" },
};

enum class Function {
    Gate        = 0,
    Retrigger   = 1,
    Length      = 2,
    Note        = 3,
    Condition   = 4,
};

static const char *functionNames[] = { "GATE", "RETRIG", "LENGTH", "NOTE", "COND" };

NoteSequencePage::NoteSequencePage(PageManager &manager, PageContext &context) :
    BasePage(manager, context),
    _contextMenu(
        manager.pages().contextMenu,
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    )
{}

void NoteSequencePage::enter() {
}

void NoteSequencePage::exit() {
    _engine.selectedTrackEngine().as<NoteTrackEngine>().clearIdleOutput();
}

void NoteSequencePage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, modeName(_mode));
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    const auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();
    const auto &sequence = _project.selectedNoteSequence();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    const auto &scale = Scale::get(sequence.scale());

    const int stepWidth = Width / StepCount;
    const int stepOffset = this->stepOffset();

    const int loopY = 16;

    // draw loop points
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    SequencePainter::drawLoopStart(canvas, (sequence.firstStep() - stepOffset) * stepWidth + 1, loopY, stepWidth - 2);
    SequencePainter::drawLoopEnd(canvas, (sequence.lastStep()  - stepOffset)* stepWidth + 1, loopY, stepWidth - 2);

    for (int i = 0; i < StepCount; ++i) {
        int stepIndex = stepOffset + i;
        const auto &step = sequence.step(stepIndex);

        int x = i * stepWidth;
        int y = 20;

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

        // step gate
        canvas.setColor(stepIndex == currentStep ? 0xf : 0x7);
        canvas.drawRect(x + 2, y + 2, stepWidth - 4, stepWidth - 4);
        if (step.gate()) {
            canvas.setColor(0xf);
            canvas.fillRect(x + 4, y + 4, stepWidth - 8, stepWidth - 8);
        }

        // gate variation
        if (_mode == Mode::GateVariation) {
            SequencePainter::drawProbability(
                canvas,
                x + 2, y + 18, stepWidth - 4, 2,
                step.gateProbability() + 1, NoteSequence::GateProbability::Range
            );
        }

        // retrigger
        if (_mode == Mode::Retrigger) {
            SequencePainter::drawRetrigger(
                canvas,
                x, y + 18, stepWidth, 2,
                step.retrigger() + 1, NoteSequence::Retrigger::Range
            );
            // canvas.setColor(0xf);
            // FixedStringBuilder<8> str("%d", step.retrigger() + 1);
            // canvas.drawText(x + (16 - canvas.textWidth(str)) / 2, y + 20, str);
        }

        // retrigger variation
        if (_mode == Mode::RetriggerVariation) {
            SequencePainter::drawProbability(
                canvas,
                x + 2, y + 18, stepWidth - 4, 2,
                step.retriggerProbability() + 1, NoteSequence::RetriggerProbability::Range
            );
        }

        // gate length
        if (_mode == Mode::Length) {
            SequencePainter::drawLength(
                canvas,
                x + 2, y + 18, stepWidth - 4, 6,
                step.length() + 1, NoteSequence::Length::Range
            );
        }

        // gate length variation
        if (_mode == Mode::LengthVariation) {
            SequencePainter::drawLengthRange(
                canvas,
                x + 2, y + 18, stepWidth - 4, 6,
                step.length() + 1, step.lengthVariationRange(), NoteSequence::Length::Range
            );
            SequencePainter::drawProbability(
                canvas,
                x + 2, y + 28, stepWidth - 4, 2,
                step.lengthVariationProbability(), NoteSequence::LengthVariationProbability::Max
            );
        }

        // note
        if (_mode == Mode::Note) {
            canvas.setColor(0xf);
            FixedStringBuilder<8> str;
            scale.shortName(step.note(), 0, str);
            canvas.drawText(x + (stepWidth - canvas.textWidth(str) + 1) / 2, y + 20, str);
            str.reset();
            scale.shortName(step.note(), 1, str);
            canvas.drawText(x + (stepWidth - canvas.textWidth(str) + 1) / 2, y + 27, str);
        }
    }

    if (_mode != Mode::Gate && os::ticks() < _lastEditTicks + os::time::ms(500)) {
        drawDetail(canvas, sequence.step(_stepSelection.first()));
    }
}

void NoteSequencePage::updateLeds(Leds &leds) {
    const auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();
    const auto &sequence = _project.selectedNoteSequence();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset() + i;
        bool red = (stepIndex == currentStep) || _stepSelection[stepIndex];
        bool green = (stepIndex != currentStep) && (sequence.step(stepIndex).gate() || _stepSelection[stepIndex]);
        leds.set(MatrixMap::fromStep(i), red, green);
    }

    LedPainter::drawSelectedSequencePage(leds, _page);
}

void NoteSequencePage::keyDown(KeyEvent &event) {
    _stepSelection.keyDown(event, stepOffset());
    updateIdleOutput();
}

void NoteSequencePage::keyUp(KeyEvent &event) {
    _stepSelection.keyUp(event, stepOffset());
    updateIdleOutput();
}

void NoteSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &sequence = _project.selectedNoteSequence();

    if (key.isContextMenu()) {
        _contextMenu.show();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    _stepSelection.keyPress(event, stepOffset());
    updateIdleOutput();

    if (!key.shiftModifier() && key.isStep()) {
        int stepIndex = stepOffset() + key.step();
        switch (_mode) {
        case Mode::Gate:
            sequence.step(stepIndex).toggleGate();
            event.consume();
            break;
        default:
            break;
        }
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Gate:
            _mode = _mode == Mode::Gate ? Mode::GateVariation : Mode::Gate;
            break;
        case Function::Retrigger:
            _mode = _mode == Mode::Retrigger ? Mode::RetriggerVariation : Mode::Retrigger;
            break;
        case Function::Length:
            _mode = _mode == Mode::Length ? Mode::LengthVariation : Mode::Length;
            break;
        case Function::Note:
            _mode = _mode == Mode::Note ? Mode::NoteVariation : Mode::Note;
            break;
        case Function::Condition:
            _mode = Mode::TrigCondition;
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

void NoteSequencePage::encoder(EncoderEvent &event) {
    auto &sequence = _project.selectedNoteSequence();
    const auto &scale = Scale::get(sequence.scale());

    if (_stepSelection.any()) {
        _lastEditTicks = os::ticks();
    } else {
        return;
    }

    const auto &firstStep = sequence.step(_stepSelection.first());

    for (size_t stepIndex = 0; stepIndex < sequence.steps().size(); ++stepIndex) {
        if (_stepSelection[stepIndex]) {
            auto &step = sequence.step(stepIndex);
            bool setToFirst = int(stepIndex) != _stepSelection.first() && _keyState[Key::Shift];
            switch (_mode) {
            case Mode::Gate:
                step.setGate(
                    setToFirst ? firstStep.gate() :
                    event.value() > 0
                );
                break;
            case Mode::GateVariation:
                step.setGateProbability(
                    setToFirst ? firstStep.gateProbability() :
                    NoteSequence::GateProbability::clamp(step.gateProbability() + event.value())
                );
                break;
            case Mode::Retrigger:
                step.setRetrigger(
                    setToFirst ? firstStep.retrigger() :
                    NoteSequence::Retrigger::clamp(step.retrigger() + event.value())
                );
                break;
            case Mode::RetriggerVariation:
                step.setRetriggerProbability(
                    setToFirst ? firstStep.retriggerProbability() :
                    NoteSequence::RetriggerProbability::clamp(step.retriggerProbability() + event.value())
                );
                break;
            case Mode::Length:
                step.setLength(
                    setToFirst ? firstStep.length() :
                    NoteSequence::Length::clamp(step.length() + event.value())
                );
                break;
            case Mode::LengthVariation:
                if (event.pressed()) {
                    step.setLengthVariationProbability(
                        setToFirst ? firstStep.lengthVariationProbability() :
                        NoteSequence::LengthVariationProbability::clamp(step.lengthVariationProbability() + event.value())
                    );
                } else {
                    step.setLengthVariationRange(
                        setToFirst ? firstStep.lengthVariationRange() :
                        NoteSequence::LengthVariationRange::clamp(step.lengthVariationRange() + event.value())
                    );
                }
                break;
            case Mode::Note:
                step.setNote(
                    setToFirst ? firstStep.note() :
                    NoteSequence::Note::clamp(step.note() + event.value() * (event.pressed() ? scale.octave() : 1))
                );
                updateIdleOutput();
                break;
            case Mode::NoteVariation:
                break;
            default:
                break;
            }
        }
    }

    event.consume();
}

void NoteSequencePage::updateIdleOutput() {
    auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();

    if (_mode == Mode::Note && _stepSelection.any()) {
        trackEngine.setIdleStep(_stepSelection.first());
        trackEngine.setIdleGate(true);
    } else {
        trackEngine.setIdleGate(false);
    }

}

void NoteSequencePage::drawDetail(Canvas &canvas, const NoteSequence::Step &step) {

    const auto &scale = Scale::get(_project.selectedNoteSequence().scale());

    FixedStringBuilder<16> str;

    WindowPainter::drawFrame(canvas, 64, 16, 128, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.vline(64 + 32, 16, 32);

    canvas.setFont(Font::Small);
    str("%d", _stepSelection.first() + 1);
    canvas.drawTextCentered(64, 16, 32, 32, str);

    canvas.setFont(Font::Tiny);

    switch (_mode) {
    case Mode::GateVariation:
        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.gateProbability() + 1, NoteSequence::GateProbability::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.gateProbability() + 1.f) / NoteSequence::GateProbability::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Mode::Retrigger:
        SequencePainter::drawRetrigger(
            canvas,
            64+ 32 + 8, 32 - 4, 64 - 16, 8,
            step.retrigger() + 1, NoteSequence::Retrigger::Range
        );
        str.reset();
        str("%d", step.retrigger() + 1);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Mode::RetriggerVariation:
        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.retriggerProbability() + 1, NoteSequence::RetriggerProbability::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.retriggerProbability() + 1.f) / NoteSequence::RetriggerProbability::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Mode::Length:
        SequencePainter::drawLength(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.length() + 1, NoteSequence::Length::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.length() + 1.f) / NoteSequence::Length::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Mode::LengthVariation:
        SequencePainter::drawLengthRange(
            canvas,
            64 + 32 + 8, 32 - 12, 64 - 16, 8,
            step.length() + 1, step.lengthVariationRange(), NoteSequence::Length::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.lengthVariationRange()) / NoteSequence::Length::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 12, 32, 8, str);

        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 + 4, 64 - 16, 8,
            step.lengthVariationProbability(), NoteSequence::LengthVariationProbability::Max
        );
        str.reset();
        str("%.1f%%", 100.f * (step.lengthVariationProbability()) / NoteSequence::LengthVariationProbability::Max);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 + 4, 32, 8, str);
        break;
    case Mode::Note:
        str.reset();
        scale.longName(step.note(), str);
        canvas.setFont(Font::Small);
        canvas.drawTextCentered(64 + 32, 16, 64, 32, str);
        break;
    default:
        break;
    }
}

void NoteSequencePage::contextAction(int index) {
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
    case ContextAction::Last:
        break;
    }
}

bool NoteSequencePage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteNoteSequence();
    default:
        return true;
    }
}

void NoteSequencePage::initSequence() {
    _project.selectedNoteSequence().clear();
}

void NoteSequencePage::copySequence() {
    _model.clipBoard().copyNoteSequence(_project.selectedNoteSequence());
}

void NoteSequencePage::pasteSequence() {
    _model.clipBoard().pasteNoteSequence(_project.selectedNoteSequence());
}

void NoteSequencePage::duplicateSequence() {
    _project.selectedNoteSequence().duplicate();
}

