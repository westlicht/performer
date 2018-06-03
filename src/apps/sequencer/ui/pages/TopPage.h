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
        Project         = Key::Track0,
        Layout          = Key::Track1,
        Track           = Key::Track2,
        Sequence        = Key::Track3,
        SequenceEdit    = Key::Track4,
        Song            = Key::Track6,
        Routing         = Key::Track7,
        Pattern         = Key::Pattern,
        Performer       = Key::Performer,
        ClockSetup      = Key::Bpm,

        // aux modes
        UserScale       = Key::Step0,
        Monitor         = Key::Step6,
        Settings        = Key::Step7,

        Last,
    };

    void setMode(Mode mode);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    Mode _mode;
    Mode _lastMode;
};
