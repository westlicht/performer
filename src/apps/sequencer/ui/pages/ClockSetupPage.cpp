#include "ClockSetupPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

ClockSetupPage::ClockSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _listModel(_project.clockSetup())
{
}

void ClockSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "CLOCK SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, "EXIT" };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void ClockSetupPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction() && key.function() == 4) {
        close();
        event.consume();
    }

    ListPage::keyPress(event);
}

