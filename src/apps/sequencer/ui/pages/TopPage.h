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

        // sequence parameters
        Scale           = 8,
        RootNote        = 9,
        Divisor         = 10,
        ResetMeasure    = 11,
        RunMode         = 12,
        FirstStep       = 13,
        LastStep        = 14,

        // aux modes
        Routing         = 20,
        UserScale       = 21,
        Monitor         = 22,
        Settings        = 23,

        Last,
    };

    void setMode(Mode mode);
    void switchMode(int direction);
    void setMainPage(Page &page);

    void setSequencePage(bool edit = false);
    void setSequencePage(Mode mode);
    void setSequenceEditPage();

    Mode _mode;
};
