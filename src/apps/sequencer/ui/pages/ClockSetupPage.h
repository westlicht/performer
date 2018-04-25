#pragma once

#include "ListPage.h"

#include "ui/model/ClockSetupListModel.h"

class ClockSetupPage : public ListPage {
public:
    ClockSetupPage(PageManager &manager, PageContext &context);

    virtual void draw(Canvas &canvas) override;

private:
    ClockSetupListModel _listModel;
};
