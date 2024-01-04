#pragma once

#include "BasePage.h"

class PerformerPage : public BasePage {
public:
    PerformerPage(PageManager &manager, PageContext &context);

    void setModal(bool modal) { _modal = modal; }

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return _modal; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void updateFills();
    bool isKeySelected();

    bool _modal = false;
    bool _latching = false;
    bool _syncing = false;
};
