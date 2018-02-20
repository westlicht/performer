#include "CurveSequenceSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

CurveSequenceSetupPage::CurveSequenceSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _listModel(_project.selectedSequence().curveSequence())
{}

void CurveSequenceSetupPage::enter() {
    // _listModel.setTrackSetup(_project.selectedTrackSetup());
}

void CurveSequenceSetupPage::exit() {
}

void CurveSequenceSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void CurveSequenceSetupPage::updateLeds(Leds &leds) {
}

void CurveSequenceSetupPage::keyDown(KeyEvent &event) {
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

void CurveSequenceSetupPage::keyUp(KeyEvent &event) {
    ListPage::keyUp(event);
}

void CurveSequenceSetupPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);
}
