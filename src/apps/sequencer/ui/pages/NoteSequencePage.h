#pragma once

#include "ListPage.h"

#include "ui/model/NoteSequenceSetupListModel.h"

class NoteSequencePage : public ListPage {
public:
    NoteSequencePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    NoteSequenceSetupListModel _listModel;
};
