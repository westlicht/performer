#pragma once

#include "BasePage.h"

class BusyPage : public BasePage {
public:
    BusyPage(PageManager &manager, PageContext &context);

    using BasePage::show;
    void show(const char *text);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    const char *_text;
};
