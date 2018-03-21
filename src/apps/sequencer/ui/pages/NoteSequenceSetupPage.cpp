#include "NoteSequenceSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

NoteSequenceSetupPage::NoteSequenceSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void NoteSequenceSetupPage::enter() {
    _listModel.setSequence(&_project.selectedNoteSequence());
}

void NoteSequenceSetupPage::exit() {
    _listModel.setSequence(nullptr);
}

void NoteSequenceSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void NoteSequenceSetupPage::updateLeds(Leds &leds) {
    const auto &sequence = _project.selectedNoteSequence();

    switch (_selectedRow) {
    case NoteSequenceSetupListModel::Scale:
        LedPainter::drawStepIndex(leds, sequence.scale());
        break;
    case NoteSequenceSetupListModel::Divisor:
        LedPainter::drawStepIndex(leds, sequence.indexedDivisor());
        break;
    case NoteSequenceSetupListModel::FirstStep:
        LedPainter::drawStepIndex(leds, sequence.firstStep());
        break;
    case NoteSequenceSetupListModel::LastStep:
        LedPainter::drawStepIndex(leds, sequence.lastStep());
        break;
    }
}

void NoteSequenceSetupPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    auto &sequence = _project.selectedNoteSequence();

    if (key.pageModifier()) {
        return;
    }

    if (key.isStep()) {
        int step = key.step();
        switch (_selectedRow) {
        case NoteSequenceSetupListModel::Scale:
            sequence.setScale(step);
            break;
        case NoteSequenceSetupListModel::Divisor:
            sequence.setIndexedDivisor(step);
            break;
        }
        event.consume();
    }

    if (!event.consumed()) {
        ListPage::keyPress(event);
    }
}
