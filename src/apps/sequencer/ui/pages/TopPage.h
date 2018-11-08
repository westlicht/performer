#pragma once

#include "BasePage.h"

#include "ui/PageKeyMap.h"

#include "model/Routing.h"

class TopPage : public BasePage {
public:
    TopPage(PageManager &manager, PageContext &context);

    void init();

    void editRoute(Routing::Target target, int trackIndex);

    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    enum Mode : uint8_t {
        // main modes
        Project         = PageKeyMap::Project,
        Layout          = PageKeyMap::Layout,
        Track           = PageKeyMap::Track,
        Sequence        = PageKeyMap::Sequence,
        SequenceEdit    = PageKeyMap::SequenceEdit,
        Song            = PageKeyMap::Song,
        Routing         = PageKeyMap::Routing,
        MidiOutput      = PageKeyMap::MidiOutput,
        Pattern         = PageKeyMap::Pattern,
        Performer       = PageKeyMap::Performer,
        Overview        = PageKeyMap::Overview,
        Clock           = PageKeyMap::Clock,

        // aux modes
        UserScale       = PageKeyMap::UserScale,
        Monitor         = PageKeyMap::Monitor,
        System          = PageKeyMap::System,

        Last,
    };

    void setMode(Mode mode);
    void setMainPage(Page &page);

    void setSequencePage();
    void setSequenceEditPage();

    Mode _mode;
    Mode _lastMode;
};
