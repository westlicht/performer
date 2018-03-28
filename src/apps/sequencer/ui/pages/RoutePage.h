#pragma once

#include "ListPage.h"

#include "ui/model/RouteListModel.h"

#include "model/Routing.h"

#include "engine/MidiLearn.h"

class RoutePage : public ListPage {
public:
    RoutePage(PageManager &manager, PageContext &context);

    using ListPage::show;
    void show(int routeIndex);
    void show(int routeIndex, const Routing::Route &editRoute);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void assignMidiLearn(const MidiLearn::Result &result);

    RouteListModel _routeListModel;
    Routing::Route *_route;
    uint8_t _routeIndex;
    Routing::Route _editRoute;
};
