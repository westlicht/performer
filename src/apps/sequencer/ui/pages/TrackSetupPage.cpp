#include "TrackSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

TrackSetupPage::TrackSetupPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void TrackSetupPage::enter() {
}

void TrackSetupPage::exit() {
}

void TrackSetupPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK SETUP");
}

void TrackSetupPage::updateLeds(Leds &leds) {
}

void TrackSetupPage::keyDown(KeyEvent &event) {
}

void TrackSetupPage::keyUp(KeyEvent &event) {
}

void TrackSetupPage::encoder(EncoderEvent &event) {
}
