#include "TrackSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

TrackSetupPage::TrackSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void TrackSetupPage::enter() {
}

void TrackSetupPage::exit() {
}

void TrackSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, "COMMIT" };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void TrackSetupPage::updateLeds(Leds &leds) {
}

void TrackSetupPage::keyDown(KeyEvent &event) {
    ListPage::keyDown(event);
}

void TrackSetupPage::keyUp(KeyEvent &event) {
    ListPage::keyUp(event);
}

void TrackSetupPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);
}
