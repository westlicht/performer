#include "ClockSetupPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Back = 4,
};

static const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, "BACK" };


ClockSetupPage::ClockSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel),
    _listModel(_project.clockSetup())
{
}

void ClockSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "CLOCK");
    WindowPainter::drawFooter(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void ClockSetupPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction() && Function(key.function()) == Function::Back) {
        close();
        event.consume();
    }

    ListPage::keyPress(event);
}
