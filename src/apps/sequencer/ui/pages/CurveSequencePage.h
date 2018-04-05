#pragma once

#include "ListPage.h"

#include "ui/model/CurveSequenceListModel.h"

class CurveSequencePage : public ListPage {
public:
    CurveSequencePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

private:
    CurveSequenceListModel _listModel;
};
