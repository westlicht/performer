#pragma once

#include "ListPage.h"

#include "ui/model/RouteListModel.h"

#include "model/Routing.h"

class RoutePage : public ListPage {
public:
    RoutePage(PageManager &manager, PageContext &context);

    using ListPage::show;
    void show(Routing::Route &route, int routeIndex);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    RouteListModel _routeListModel;
    Routing::Route *_route;
    uint8_t _routeIndex;
    Routing::Route _editRoute;
};
