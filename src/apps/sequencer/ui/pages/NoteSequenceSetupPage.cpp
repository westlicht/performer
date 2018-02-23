#include "NoteSequenceSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

NoteSequenceSetupPage::NoteSequenceSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void NoteSequenceSetupPage::enter() {
    _listModel.setSequence(&_project.selectedSequence().noteSequence());
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
}
