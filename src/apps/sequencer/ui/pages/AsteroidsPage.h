#pragma once

#include "BasePage.h"

#include "asteroids/Asteroids.h"

class AsteroidsPage : public BasePage {
public:
    AsteroidsPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual bool isModal() const override { return true; }

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    asteroids::Game _game;
    asteroids::Inputs _keyInputs;

    uint32_t _lastTicks;
};
