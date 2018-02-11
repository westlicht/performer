#include "ProjectPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

ProjectPage::ProjectPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void ProjectPage::enter() {
}

void ProjectPage::exit() {
}

void ProjectPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PROJECT");

    const char *functionNames[] = { "LOAD", "SAVE", nullptr, nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);
}

void ProjectPage::updateLeds(Leds &leds) {
}

void ProjectPage::keyDown(KeyEvent &event) {
}

void ProjectPage::keyUp(KeyEvent &event) {
}

void ProjectPage::encoder(EncoderEvent &event) {
}
