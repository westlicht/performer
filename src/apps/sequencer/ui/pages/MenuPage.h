#pragma once

#include "BasePage.h"

class MenuPage : public BasePage {
public:
    struct Options {
        const char *title;
        uint8_t itemCount;
    };

    MenuPage(PageManager &manager, PageContext &context, Options &options);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyDown(KeyEvent &event) override;
    virtual void keyUp(KeyEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

protected:
    void drawItemName(Canvas &canvas, const char *name);
    void drawItemValue(Canvas &canvas, const char *value);
    void drawItemValue(Canvas &canvas, int value);
    void drawItemValue(Canvas &canvas, bool value);

    Options &_options;
    uint8_t _selectedItem = 0;
};
