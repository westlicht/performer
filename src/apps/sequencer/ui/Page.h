#pragma once

#include "Config.h"

#include "Event.h"
#include "Leds.h"

#include "core/gfx/Canvas.h"

class PageManager;

class Page {
public:
    Page(PageManager &manager);

    virtual void enter() {}
    virtual void exit() {}

    virtual void draw(Canvas &canvas) {}
    virtual void updateLeds(Leds &leds) {}

    // Event handlers
    virtual void keyDown(KeyEvent &event) {}
    virtual void keyUp(KeyEvent &event) {}
    virtual void encoder(EncoderEvent &event) {}

    virtual void show();
    virtual void close();
    virtual bool isTop();

    virtual void dispatchEvent(Event &event);

protected:
    static constexpr int Width = CONFIG_LCD_WIDTH;
    static constexpr int Height = CONFIG_LCD_HEIGHT;

    PageManager &_manager;
};
