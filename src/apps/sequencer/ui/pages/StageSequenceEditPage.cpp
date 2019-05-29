#include "StageSequenceEditPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "model/Scale.h"

#include "os/os.h"

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
    Stage       = 0,
    Gate0       = 1,
    Note0       = 2,
    Gate1       = 3,
    Note1       = 4,
};

static const char *functionNames[] = { "STAGE", "GATE 1", "NOTE 1", "GATE 2", "NOTE 2" };

static const StageSequenceListModel::Item quickEditItems[8] = {
    StageSequenceListModel::Item::FirstStep,
    StageSequenceListModel::Item::LastStep,
    StageSequenceListModel::Item::RunMode,
    StageSequenceListModel::Item::Divisor,
    StageSequenceListModel::Item::ResetMeasure,
    StageSequenceListModel::Item::Scale,
    StageSequenceListModel::Item::RootNote,
    StageSequenceListModel::Item::Last
};

StageSequenceEditPage::StageSequenceEditPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void StageSequenceEditPage::enter() {
    updateMonitorStep();

    _showDetail = false;
}

void StageSequenceEditPage::exit() {
}

void StageSequenceEditPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "STEPS");
    WindowPainter::drawActiveFunction(canvas, StageSequence::layerName(layer()));
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState(), activeFunctionKey());

    const auto &trackEngine = _engine.selectedTrackEngine().as<StageTrackEngine>();
    const auto &sequence = _project.selectedStageSequence();
    const auto &scale = sequence.selectedScale(_project.scale());
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;
    // int currentRecordStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentRecordStep() : -1;

    const int stepWidth = Width / StepCount;
    const int stepOffset = this->stepOffset();

    const int loopY = 16;

    // draw loop points
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    SequencePainter::drawLoopStart(canvas, (sequence.firstStep() - stepOffset) * stepWidth + 1, loopY, stepWidth - 2);
    SequencePainter::drawLoopEnd(canvas, (sequence.lastStep() - stepOffset) * stepWidth + 1, loopY, stepWidth - 2);

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
        // if (step.gate()) {
        //     canvas.setColor(0xf);
        //     canvas.fillRect(x + 4, y + 4, stepWidth - 8, stepWidth - 8);
        // }

#if 0
        // record step
        if (layer() == Layer::Note && stepIndex == currentRecordStep) {
            // draw circle
            canvas.setColor(step.gate() ? 0x0 : 0xf);
            canvas.fillRect(x + 6, y + 6, stepWidth - 12, stepWidth - 12);
            canvas.setColor(0x7);
            canvas.hline(x + 7, y + 5, 2);
            canvas.hline(x + 7, y + 10, 2);
            canvas.vline(x + 5, y + 7, 2);
            canvas.vline(x + 10, y + 7, 2);
        }
#endif
        switch (layer()) {
        case Layer::Length: {
            canvas.setColor(0xf);
            FixedStringBuilder<8> str("%d", step.length());
            canvas.drawText(x + (stepWidth - canvas.textWidth(str) + 1) / 2, y + 20, str);
            break;
        }
        case Layer::GateCount0:
        case Layer::GateCount1: {
            int channel = layer() == Layer::GateCount0 ? 0 : 1;
            canvas.setColor(0xf);
            FixedStringBuilder<8> str("%d", step.gateCount(channel));
            canvas.drawText(x + (stepWidth - canvas.textWidth(str) + 1) / 2, y + 20, str);
            break;
        }
        case Layer::Note0:
        case Layer::Note1: {
            int channel = layer() == Layer::Note0 ? 0 : 1;
            int rootNote = sequence.selectedRootNote(_model.project().rootNote());
            int note = step.note(channel) + rootNote;
            canvas.setColor(0xf);
            FixedStringBuilder<8> str;
            scale.noteName(str, note, Scale::Short1);
            canvas.drawText(x + (stepWidth - canvas.textWidth(str) + 1) / 2, y + 20, str);
            str.reset();
            scale.noteName(str, note, Scale::Short2);
            canvas.drawText(x + (stepWidth - canvas.textWidth(str) + 1) / 2, y + 27, str);
            break;
        }
        default:
            break;
        }
    }

    // handle detail display

#if 0
    if (_showDetail) {
        if (layer() == Layer::Gate || layer() == Layer::Slide || _stepSelection.none()) {
            _showDetail = false;
        }
        if (_stepSelection.isPersisted() && os::ticks() > _showDetailTicks + os::time::ms(500)) {
            _showDetail = false;
        }
    }

    if (_showDetail) {
        drawDetail(canvas, sequence.step(_stepSelection.first()));
    }
#endif
}

void StageSequenceEditPage::updateLeds(Leds &leds) {
#if 0
    const auto &trackEngine = _engine.selectedTrackEngine().as<StageTrackEngine>();
    const auto &sequence = _project.selectedStageSequence();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset() + i;
        bool red = (stepIndex == currentStep) || _stepSelection[stepIndex];
        bool green = (stepIndex != currentStep) && (sequence.step(stepIndex).gate() || _stepSelection[stepIndex]);
        leds.set(MatrixMap::fromStep(i), red, green);
    }

    LedPainter::drawSelectedSequenceSection(leds, _section);

    // show quick edit keys
    if (globalKeyState()[Key::Page] && !globalKeyState()[Key::Shift]) {
        for (int i = 0; i < 8; ++i) {
            int index = MatrixMap::fromStep(i + 8);
            leds.unmask(index);
            leds.set(index, false, quickEditItems[i] != StageSequenceListModel::Item::Last);
            leds.mask(index);
        }
    }
#endif
}

void StageSequenceEditPage::keyDown(KeyEvent &event) {
    _stepSelection.keyDown(event, stepOffset());
    updateMonitorStep();
}

void StageSequenceEditPage::keyUp(KeyEvent &event) {
    _stepSelection.keyUp(event, stepOffset());
    updateMonitorStep();
}

void StageSequenceEditPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &sequence = _project.selectedStageSequence();

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
    updateMonitorStep();
#if 0
    if (!key.shiftModifier() && key.isStep()) {
        int stepIndex = stepOffset() + key.step();
        switch (layer()) {
        case Layer::Gate:
            sequence.step(stepIndex).toggleGate();
            event.consume();
            break;
        default:
            break;
        }
    }
#endif
    if (key.isFunction()) {
        switchLayer(key.function());
        event.consume();
    }

    if (key.isEncoder()) {
        if (!_showDetail && _stepSelection.any() && allSelectedStepsActive()) {
            setSelectedStepsGate(false);
        } else {
            setSelectedStepsGate(true);
        }
    }

    if (key.isLeft()) {
        if (key.shiftModifier()) {
            sequence.shiftSteps(-1);
        } else {
            _section = std::max(0, _section - 1);
        }
        event.consume();
    }
    if (key.isRight()) {
        if (key.shiftModifier()) {
            sequence.shiftSteps(1);
        } else {
            _section = std::min(3, _section + 1);
        }
        event.consume();
    }
}

void StageSequenceEditPage::encoder(EncoderEvent &event) {
    auto &sequence = _project.selectedStageSequence();

    if (_stepSelection.any()) {
        _showDetail = true;
        _showDetailTicks = os::ticks();
    } else {
        return;
    }

    const auto &firstStep = sequence.step(_stepSelection.first());

    // TODO this is incorrect if first step index is higher than other step indices (shift mode -> setToFirst)
    for (size_t stepIndex = 0; stepIndex < sequence.steps().size(); ++stepIndex) {
        if (_stepSelection[stepIndex]) {
            auto &step = sequence.step(stepIndex);
            bool setToFirst = int(stepIndex) != _stepSelection.first() && globalKeyState()[Key::Shift];
            switch (layer()) {
            case Layer::Length:
                step.setLength(
                    setToFirst ? firstStep.length() :
                    step.length() + event.value()
                );
                break;
            case Layer::GateCount0:
            case Layer::GateCount1: {
                int channel = layer() == Layer::GateCount0 ? 0 : 1;
                step.setGateCount(
                    channel,
                    setToFirst ? firstStep.gateCount(channel) :
                    step.gateCount(channel) + event.value()
                );
                break;
            }
            case Layer::Note0:
            case Layer::Note1: {
                int channel = layer() == Layer::Note0 ? 0 : 1;
                step.setNote(
                    channel,
                    setToFirst ? firstStep.note(channel) :
                    step.note(channel) + event.value()
                );
                // updateMonitorStep();
                break;
            }

            // case Layer::LengthVariationRange:
            //     step.setLengthVariationRange(
            //         setToFirst ? firstStep.lengthVariationRange() :
            //         step.lengthVariationRange() + event.value()
            //     );
            //     break;
            // case Layer::LengthVariationProbability:
            //     step.setLengthVariationProbability(
            //         setToFirst ? firstStep.lengthVariationProbability() :
            //         step.lengthVariationProbability() + event.value()
            //     );
            //     break;
            // case Layer::NoteVariationRange:
            //     step.setNoteVariationRange(
            //         setToFirst ? firstStep.noteVariationRange() :
            //         step.noteVariationRange() + event.value()
            //     );
            //     updateMonitorStep();
            //     break;
            // case Layer::NoteVariationProbability:
            //     step.setNoteVariationProbability(
            //         setToFirst ? firstStep.noteVariationProbability() :
            //         step.noteVariationProbability() + event.value()
            //     );
            //     break;
            // case Layer::Slide:
            //     step.setSlide(
            //         setToFirst ? firstStep.slide() :
            //         event.value() > 0
            //     );
            //     break;
            default:
                break;
            }
        }
    }

    event.consume();
}

void StageSequenceEditPage::midi(MidiEvent &event) {
#if 0
    if (!_engine.recording() && layer() == Layer::Note && _stepSelection.any()) {
        auto &trackEngine = _engine.selectedTrackEngine().as<StageTrackEngine>();
        auto &sequence = _project.selectedStageSequence();
        const auto &scale = sequence.selectedScale(_project.scale());
        const auto &message = event.message();

        if (message.isNoteOn()) {
            float volts = (message.note() - 60) * (1.f / 12.f);
            int note = scale.noteFromVolts(volts);

            for (size_t stepIndex = 0; stepIndex < sequence.steps().size(); ++stepIndex) {
                if (_stepSelection[stepIndex]) {
                    auto &step = sequence.step(stepIndex);
                    step.setNote(note);
                    step.setGate(true);
                }
            }

            trackEngine.setMonitorStep(_stepSelection.first());
            updateMonitorStep();
        }
    }
#endif
}

void StageSequenceEditPage::switchLayer(int functionKey) {
    switch (Function(functionKey)) {
    case Function::Stage:
        switch (layer()) {
        default:
            setLayer(Layer::Length);
            break;
        }
        break;
    case Function::Gate0:
        switch (layer()) {
        default:
            setLayer(Layer::GateCount0);
            break;
        }
        break;
    case Function::Note0:
        switch (layer()) {
        default:
            setLayer(Layer::Note0);
            break;
        }
        break;
    case Function::Gate1:
        switch (layer()) {
        default:
            setLayer(Layer::GateCount1);
            break;
        }
        break;
    case Function::Note1:
        switch (layer()) {
        default:
            setLayer(Layer::Note1);
            break;
        }
        break;
    }
}

int StageSequenceEditPage::activeFunctionKey() {
    switch (layer()) {
    case Layer::Length:
        return 0;
    case Layer::GateCount0:
        return 1;
    case Layer::Note0:
        return 2;
    case Layer::GateCount1:
        return 3;
    case Layer::Note1:
        return 4;
    case Layer::Last:
        break;
    }

    return -1;
}

void StageSequenceEditPage::updateMonitorStep() {
#if 0
    auto &trackEngine = _engine.selectedTrackEngine().as<StageTrackEngine>();

    if (layer() == Layer::Note && !_stepSelection.isPersisted() && _stepSelection.any()) {
        trackEngine.setMonitorStep(_stepSelection.first());
    } else {
        trackEngine.setMonitorStep(-1);
    }
#endif
}

void StageSequenceEditPage::drawDetail(Canvas &canvas, const StageSequence::Step &step) {
#if 0
    const auto &sequence = _project.selectedStageSequence();
    const auto &scale = sequence.selectedScale(_project.scale());

    FixedStringBuilder<16> str;

    WindowPainter::drawFrame(canvas, 64, 16, 128, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.vline(64 + 32, 16, 32);

    canvas.setFont(Font::Small);
    str("%d", _stepSelection.first() + 1);
    if (_stepSelection.count() > 1) {
        str("*");
    }
    canvas.drawTextCentered(64, 16, 32, 32, str);

    canvas.setFont(Font::Tiny);

    switch (layer()) {
    case Layer::GateProbability:
        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.gateProbability() + 1, StageSequence::GateProbability::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.gateProbability() + 1.f) / StageSequence::GateProbability::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Layer::GateOffset:
        SequencePainter::drawOffset(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.gateOffset(), StageSequence::GateOffset::Min - 1, StageSequence::GateOffset::Max + 1
        );
        str.reset();
        str("%.1f%%", 100.f * step.gateOffset() / float(StageSequence::GateOffset::Max + 1));
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Layer::Retrigger:
        SequencePainter::drawRetrigger(
            canvas,
            64+ 32 + 8, 32 - 4, 64 - 16, 8,
            step.retrigger() + 1, StageSequence::Retrigger::Range
        );
        str.reset();
        str("%d", step.retrigger() + 1);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Layer::RetriggerProbability:
        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.retriggerProbability() + 1, StageSequence::RetriggerProbability::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.retriggerProbability() + 1.f) / StageSequence::RetriggerProbability::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Layer::Length:
        SequencePainter::drawLength(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.length() + 1, StageSequence::Length::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.length() + 1.f) / StageSequence::Length::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Layer::LengthVariationRange:
        SequencePainter::drawLengthRange(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.length() + 1, step.lengthVariationRange(), StageSequence::Length::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.lengthVariationRange()) / StageSequence::Length::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Layer::LengthVariationProbability:
        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.lengthVariationProbability() + 1, StageSequence::LengthVariationProbability::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.lengthVariationProbability() + 1.f) / StageSequence::LengthVariationProbability::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Layer::Note:
        str.reset();
        scale.noteName(str, step.note() + sequence.selectedRootNote(_model.project().rootNote()), Scale::Long);
        canvas.setFont(Font::Small);
        canvas.drawTextCentered(64 + 32, 16, 64, 32, str);
        break;
    case Layer::NoteVariationRange:
        str.reset();
        str("%d", step.noteVariationRange());
        canvas.setFont(Font::Small);
        canvas.drawTextCentered(64 + 32, 16, 64, 32, str);
        break;
    case Layer::NoteVariationProbability:
        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.noteVariationProbability() + 1, StageSequence::NoteVariationProbability::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.noteVariationProbability() + 1.f) / StageSequence::NoteVariationProbability::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;

    default:
        break;
    }
#endif
}

void StageSequenceEditPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void StageSequenceEditPage::contextAction(int index) {
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

bool StageSequenceEditPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteStageSequenceSteps();
    default:
        return true;
    }
}

void StageSequenceEditPage::initSequence() {
    _project.selectedStageSequence().clearSteps();
    showMessage("STEPS INITIALIZED");
}

void StageSequenceEditPage::copySequence() {
    _model.clipBoard().copyStageSequenceSteps(_project.selectedStageSequence(), _stepSelection.selected());
    showMessage("STEPS COPIED");
}

void StageSequenceEditPage::pasteSequence() {
    _model.clipBoard().pasteStageSequenceSteps(_project.selectedStageSequence(), _stepSelection.selected());
    showMessage("STEPS PASTED");
}

void StageSequenceEditPage::duplicateSequence() {
    _project.selectedStageSequence().duplicateSteps();
    showMessage("STEPS DUPLICATED");
}

void StageSequenceEditPage::generateSequence() {
    // _manager.pages().generatorSelect.show([this] (bool success, Generator::Mode mode) {
    //     if (success) {
    //         auto builder = _builderContainer.create<StageSequenceBuilder>(_project.selectedStageSequence(), layer());
    //         auto generator = Generator::create(mode, *builder);
    //         _manager.pages().generator.show(generator);
    //     }
    // });
}

void StageSequenceEditPage::quickEdit(int index) {
    _listModel.setSequence(&_project.selectedStageSequence());
    if (quickEditItems[index] != StageSequenceListModel::Item::Last) {
        _manager.pages().quickEdit.show(_listModel, int(quickEditItems[index]));
    }
}

bool StageSequenceEditPage::allSelectedStepsActive() const {
#if 0
    const auto &sequence = _project.selectedStageSequence();
    for (size_t stepIndex = 0; stepIndex < _stepSelection.size(); ++stepIndex) {
        if (_stepSelection[stepIndex] && !sequence.step(stepIndex).gate()) {
            return false;
        }
    }
    return true;
#endif
}

void StageSequenceEditPage::setSelectedStepsGate(bool gate) {
#if 0
    auto &sequence = _project.selectedStageSequence();
    for (size_t stepIndex = 0; stepIndex < _stepSelection.size(); ++stepIndex) {
        if (_stepSelection[stepIndex]) {
            sequence.step(stepIndex).setGate(gate);
        }
    }
#endif
}
