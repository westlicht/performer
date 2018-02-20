#include "NoteSequenceSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

NoteSequenceSetupPage::NoteSequenceSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _listModel(_project.selectedSequence().noteSequence())
{}

void NoteSequenceSetupPage::enter() {
    // _listModel.setTrackSetup(_project.selectedTrackSetup());
}

void NoteSequenceSetupPage::exit() {
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

void NoteSequenceSetupPage::keyDown(KeyEvent &event) {
    // const auto &key = event.key();

    // if (key.isFunction() && key.function() == 4) {
    //     _project.setTrackSetup(_project.selectedTrackIndex(), _listModel.trackSetup());
    // }

    // if (key.isTrackSelect()) {
    //     _project.setSelectedTrackIndex(key.trackSelect());
    //     _listModel.setTrackSetup(_project.selectedTrackSetup());
    // }

    ListPage::keyDown(event);
}

void NoteSequenceSetupPage::keyUp(KeyEvent &event) {
    ListPage::keyUp(event);
}

void NoteSequenceSetupPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);
}
