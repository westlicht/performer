#include "HarmonySequenceEditPage.h"

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
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUPL" },
};

enum class Function {
    Prev        = 0,
    Next        = 1,
    Add         = 2,
    Insert      = 3,
    Remove      = 4,
};

static const char *functionNames[] = { "PREV", "NEXT", "ADD", "INSERT", "REMOVE" };

static const HarmonySequenceListModel::Item quickEditItems[8] = {
    HarmonySequenceListModel::Item::FirstStep,
    HarmonySequenceListModel::Item::LastStep,
    HarmonySequenceListModel::Item::RunMode,
    HarmonySequenceListModel::Item::Divisor,
    HarmonySequenceListModel::Item::ResetMeasure,
    HarmonySequenceListModel::Item::Last,
    HarmonySequenceListModel::Item::Last,
    HarmonySequenceListModel::Item::Last
};

HarmonySequenceEditPage::HarmonySequenceEditPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _stepListModel)
{}

void HarmonySequenceEditPage::enter() {
    updateSelectedStep();
}

void HarmonySequenceEditPage::exit() {
}

void HarmonySequenceEditPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "STEPS");
    WindowPainter::drawActiveFunction(canvas, FixedStringBuilder<16>("STEP %d", _stepSelection.first() + 1));
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    ListPage::draw(canvas);
}

void HarmonySequenceEditPage::updateLeds(Leds &leds) {
    const auto &trackEngine = _engine.selectedTrackEngine().as<HarmonyTrackEngine>();
    const auto &sequence = _project.selectedHarmonySequence();
    int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;

    for (int i = 0; i < 16; ++i) {
        int stepIndex = i;
        bool red = (stepIndex == currentStep) || _stepSelection[stepIndex];
        bool green = (stepIndex != currentStep) && _stepSelection[stepIndex];
        leds.set(MatrixMap::fromStep(i), red, green);
    }

    // LedPainter::drawSelectedSequenceSection(leds, _section);

    // show quick edit keys
    if (globalKeyState()[Key::Page] && !globalKeyState()[Key::Shift]) {
        for (int i = 0; i < 8; ++i) {
            int index = MatrixMap::fromStep(i + 8);
            leds.unmask(index);
            leds.set(index, false, quickEditItems[i] != HarmonySequenceListModel::Item::Last);
            leds.mask(index);
        }
    }
}

void HarmonySequenceEditPage::keyDown(KeyEvent &event) {
    _stepSelection.keyDown(event, 0);
    updateSelectedStep();
}

void HarmonySequenceEditPage::keyUp(KeyEvent &event) {
    _stepSelection.keyUp(event, 0);
    updateSelectedStep();
}

void HarmonySequenceEditPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    // auto &sequence = _project.selectedHarmonySequence();

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

    _stepSelection.keyPress(event, 0);
    updateSelectedStep();

#if 0
    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Prev:
            setStepIndex(_stepIndex - 1);
            break;
        case Function::Next:
            setStepIndex(_stepIndex + 1);
            break;
        case Function::Add:
            sequence.addStep();
            setStepIndex(sequence.numSteps() - 1);
            break;
        case Function::Remove:
            sequence.removeStep(_stepIndex);
            setStepIndex(_stepIndex);
            break;
        case Function::Insert:
            sequence.insertStep(_stepIndex);
            setStepIndex(_stepIndex);
            break;
        }
        event.consume();
        return;
    }
#endif

    ListPage::keyPress(event);
}

void HarmonySequenceEditPage::encoder(EncoderEvent &event) {
    // if (globalKeyState()[Key::Shift]) {
    //     setStepIndex(_stepIndex + event.value());
    //     event.consume();
    // } else {
        ListPage::encoder(event);
    // }
}

void HarmonySequenceEditPage::midi(MidiEvent &event) {
}

void HarmonySequenceEditPage::drawCell(Canvas &canvas, int row, int column, int x, int y, int w, int h) {
    ListPage::drawCell(canvas, row, column, x, y, w, h);
}

void HarmonySequenceEditPage::updateSelectedStep() {
    auto &sequence = _project.selectedHarmonySequence();
    int stepIndex = _stepSelection.first();
    _stepListModel.setStep(stepIndex >= 0 ? &sequence.step(stepIndex) : nullptr);
}

void HarmonySequenceEditPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void HarmonySequenceEditPage::contextAction(int index) {
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

bool HarmonySequenceEditPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPasteNoteSequenceSteps();
    default:
        return true;
    }
}

void HarmonySequenceEditPage::initSequence() {
    _project.selectedHarmonySequence().clearSteps();
    _stepSelection.clear();
    updateSelectedStep();
    showMessage("STEPS INITIALIZED");
}

void HarmonySequenceEditPage::copySequence() {
    // _model.clipBoard().copyNoteSequenceSteps(_project.selectedHarmonySequence(), _stepSelection.selected());
    showMessage("STEPS COPIED");
}

void HarmonySequenceEditPage::pasteSequence() {
    // _model.clipBoard().pasteNoteSequenceSteps(_project.selectedHarmonySequence(), _stepSelection.selected());
    showMessage("STEPS PASTED");
}

void HarmonySequenceEditPage::duplicateSequence() {
    _project.selectedHarmonySequence().duplicateSteps();
    showMessage("STEPS DUPLICATED");
}

void HarmonySequenceEditPage::quickEdit(int index) {
    _sequenceListModel.setSequence(&_project.selectedHarmonySequence());
    if (quickEditItems[index] != HarmonySequenceListModel::Item::Last) {
        _manager.pages().quickEdit.show(_sequenceListModel, int(quickEditItems[index]));
    }
}
