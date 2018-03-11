#pragma once

#include "BasePage.h"

class TopPage : public BasePage {
public:
    TopPage(PageManager &manager, PageContext &context);

    void init();

    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    enum Mode : uint8_t {
        // main modes
        Project         = 0,
        Pattern         = 1,
        TrackSetup      = 2,
        Sequence        = 3,
        SequenceSetup   = 4,
        Performer       = 7,

        // sequence setup shortcuts
        Scale           = 8,
        Divisor         = 9,
        ResetMeasure    = 10,
        PlayMode        = 11,
        FirstStep       = 12,

        // aux modes
        Monitor         = 23,

        Last,
    };

    void setMode(Mode mode);
    void switchMode(int direction);
    void setMainPage(Page &page);

    bool isSequencePage();
    void setSequencePage();

    bool isSequenceSetupPage();
    void setSequenceSetupPage();
    void setSequenceSetupPage(int row);

    Mode _mode;
};
