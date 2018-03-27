#pragma once

#include "ListPage.h"

#include "ui/model/RoutingListModel.h"

#include "model/Routing.h"

class RoutingPage : public ListPage {
public:
    RoutingPage(PageManager &manager, PageContext &context);

    void initRoute(Routing::Param param, int trackIndex);

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    RoutingListModel _routingListModel;
};
