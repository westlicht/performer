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

private:
    const char *_text;
};
