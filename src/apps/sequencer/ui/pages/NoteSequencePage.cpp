#include "NoteSequencePage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

NoteSequencePage::NoteSequencePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void NoteSequencePage::enter() {
    _listModel.setSequence(&_project.selectedNoteSequence());
}

void NoteSequencePage::exit() {
    _listModel.setSequence(nullptr);
}

void NoteSequencePage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void NoteSequencePage::updateLeds(Leds &leds) {
    const auto &sequence = _project.selectedNoteSequence();

    switch (selectedRow()) {
    case NoteSequenceListModel::Scale:
        LedPainter::drawStepIndex(leds, sequence.scale());
        break;
    case NoteSequenceListModel::Divisor:
        LedPainter::drawStepIndex(leds, sequence.indexedDivisor());
        break;
    case NoteSequenceListModel::FirstStep:
        LedPainter::drawStepIndex(leds, sequence.firstStep());
        break;
    case NoteSequenceListModel::LastStep:
        LedPainter::drawStepIndex(leds, sequence.lastStep());
        break;
    }
}

void NoteSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    auto &sequence = _project.selectedNoteSequence();

    if (key.pageModifier()) {
        return;
    }

    if (key.isStep()) {
        int step = key.step();
        switch (selectedRow()) {
        case NoteSequenceListModel::Scale:
            sequence.setScale(step);
            break;
        case NoteSequenceListModel::Divisor:
            sequence.setIndexedDivisor(step);
            break;
        }
        event.consume();
    }

    if (!event.consumed()) {
        ListPage::keyPress(event);
    }
}
