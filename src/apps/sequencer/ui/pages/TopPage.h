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
        Project         = 0,
        Pattern         = 1,
        TrackSetup      = 2,
        Sequence        = 3,
        SequenceSetup   = 4,
        Performer       = 7,
        Monitor         = 23,
    };

    void setMode(Mode mode);
    void setMainPage(Page &page);

    bool isSequencePage();
    void setSequencePage();

    bool isSequenceSetupPage();
    void setSequenceSetupPage();

    Mode _mode;
};
