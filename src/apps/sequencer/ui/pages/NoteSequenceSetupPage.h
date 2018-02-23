#pragma once

#include "ListPage.h"

#include "ui/model/NoteSequenceSetupListModel.h"

class NoteSequenceSetupPage : public ListPage {
public:
    NoteSequenceSetupPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

private:
    NoteSequenceSetupListModel _listModel;
};
