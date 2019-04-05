#pragma once

#include "ListPage.h"

#include "ui/model/NoteSequenceListModel.h"

class NoteSequencePage : public ListPage {
public:
    NoteSequencePage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void contextShow();
    void contextAction(int index);
    bool contextActionEnabled(int index) const;

    void initSequence();
    void copySequence();
    void pasteSequence();
    void duplicateSequence();
    void initRoute();

    NoteSequenceListModel _listModel;
};
