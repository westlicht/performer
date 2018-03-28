#include "RoutingPage.h"

#include "Pages.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"


RoutingPage::RoutingPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _routingListModel),
    _routingListModel(_project.routing())
{
}

void RoutingPage::initRoute(Routing::Param param, int trackIndex) {

}

void RoutingPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "ROUTING");

    ListPage::draw(canvas);
}

void RoutingPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isEncoder()) {
        _manager.pages().route.show(selectedRow());
        event.consume();
        return;
    }

    ListPage::keyPress(event);
}

