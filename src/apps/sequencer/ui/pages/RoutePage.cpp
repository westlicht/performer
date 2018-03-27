#include "RoutePage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Change  = 0,
    Learn   = 1,
    Back    = 4,
};

RoutePage::RoutePage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _routeListModel),
    _routeListModel(_editRoute)
{
}

void RoutePage::show(Routing::Route &route, int routeIndex) {
    _route = &route;
    _routeIndex = routeIndex;
    _editRoute = *_route;
    _selectedRow = 0;
    _edit = false;
    ListPage::show();
}

void RoutePage::enter() {
    ListPage::enter();
}

void RoutePage::exit() {
    ListPage::exit();
}

void RoutePage::draw(Canvas &canvas) {
    bool showChange = *_route != _editRoute;
    const char *functionNames[] = { showChange ? "CHANGE" : nullptr, "LEARN", nullptr, nullptr, "BACK" };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ROUTING");
    WindowPainter::drawActiveFunction(canvas, FixedStringBuilder<16>("ROUTE %d", _routeIndex + 1));
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void RoutePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Change:
            *_route = _editRoute;
            break;
        case Function::Learn:
            // _engine.midiLearn().start([] (const MidiLearn::Result &result) {
            //     DBG("learned!");
            // });
            // _engine.midiLearn().stop();
            break;
        case Function::Back:
            close();
            break;
        }
        event.consume();
    }

    ListPage::keyPress(event);
}
