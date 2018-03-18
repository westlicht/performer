#pragma once

#include "ListPage.h"
#include "ContextMenu.h"

#include "ui/model/TrackSetupListModel.h"

class TrackSetupPage : public ListPage {
public:
    TrackSetupPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;

private:
    void contextAction(int index);
    bool contextActionEnabled(int index) const;
    void initTrackSetup();
    void copyTrackSetup();
    void pasteTrackSetup();

    TrackSetupListModel _listModel;
    ContextMenu _contextMenu;
};
