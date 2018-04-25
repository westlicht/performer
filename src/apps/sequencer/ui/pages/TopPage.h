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
        Project         = Key::Code::Track0,
        Layout          = Key::Code::Track1,
        Track           = Key::Code::Track2,
        Sequence        = Key::Code::Track3,
        SequenceEdit    = Key::Code::Track4,
        Routing         = Key::Code::Track7,
        Pattern         = Key::Code::Pattern,
        Performer       = Key::Code::Performer,
        ClockSetup      = Key::Code::Bpm,

        // aux modes
        UserScale       = Key::Code::Step0,
        Monitor         = Key::Code::Step1,
        Settings        = Key::Code::Step7,

        Last,
    };

    void setMode(Mode mode);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    Mode _mode;
    Mode _lastMode;
};
