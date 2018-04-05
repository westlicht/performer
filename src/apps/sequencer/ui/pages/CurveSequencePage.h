#pragma once

#include "ListPage.h"

#include "ui/model/CurveSequenceSetupListModel.h"

class CurveSequencePage : public ListPage {
public:
    CurveSequencePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

private:
    CurveSequenceSetupListModel _listModel;
};
