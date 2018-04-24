#pragma once

#include "BasePage.h"

#include "model/Routing.h"

class TopPage : public BasePage {
public:
    TopPage(PageManager &manager, PageContext &context);

    void init();

    void editRoute(Routing::Param param, int trackIndex);

    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    enum Mode : uint8_t {
        // main modes
        Project         = 0,
        Layout          = 1,
        Track           = 2,
        Sequence        = 3,
        SequenceEdit    = 4,
        Pattern         = 6,
        Performer       = 7,

        // aux modes
        Routing         = 8,
        UserScale       = 9,
        Monitor         = 10,
        Settings        = 11,

        Last,
    };

    void setMode(Mode mode);
    void navigateMode(int direction);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    Mode _mode;
};
