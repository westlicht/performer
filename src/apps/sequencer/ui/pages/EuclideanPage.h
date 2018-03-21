#pragma once

#include "BasePage.h"

#include "engine/generators/Rhythm.h"

class EuclideanPage : public BasePage {
public:
    EuclideanPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void updatePattern();

    int _steps = 16;
    int _beats = 4;
    int _offset = 0;
    Rhythm::Pattern _pattern;
};
