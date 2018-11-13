#pragma once

#include "BasePage.h"

class StartupPage : public BasePage {
public:
    StartupPage(PageManager &manager, PageContext &context);

    virtual void enter() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

private:
    uint32_t _startTicks;
    bool _ready = false;
};
