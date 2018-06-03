#pragma once

#include "ListPage.h"

#include "ui/model/RoutingListModel.h"

#include "model/Routing.h"

class RoutingPage : public ListPage {
public:
    RoutingPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    RoutingListModel _routingListModel;
};
