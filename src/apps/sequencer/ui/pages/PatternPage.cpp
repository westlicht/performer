#include "PatternPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

PatternPage::PatternPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void PatternPage::enter() {
}

void PatternPage::exit() {
}

void PatternPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PATTERN");

    const char *functionNames[] = { "LOAD", "SAVE", nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);
}

void PatternPage::updateLeds(Leds &leds) {
}

void PatternPage::keyDown(KeyEvent &event) {
}

void PatternPage::keyUp(KeyEvent &event) {
}

void PatternPage::encoder(EncoderEvent &event) {
}
