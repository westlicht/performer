#pragma once

#include "Config.h"

#include "Event.h"
#include "Leds.h"

#include "core/gfx/Canvas.h"

#include "drivers/ButtonLedMatrix.h"

struct Pages;

class PageManager;

class Page {
public:
    Page(PageManager &pageManager);

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
    virtual void dispatchEvent(Event &event);

protected:
    static constexpr int PageWidth = CONFIG_LCD_WIDTH;
    static constexpr int PageHeight = CONFIG_LCD_HEIGHT;

    PageManager &_pageManager;
};

class PageManager {
public:
    PageManager(Pages &pages);

    Pages &pages() { return _pages; }

    Page &top();
    void push(Page &page);
    void pop();
    void reset(Page &page);

    void draw(Canvas &canvas);
    void updateLeds(Leds &leds);
    void dispatchEvent(Event &event);

private:
    Pages &_pages;
    static const int PageStackSize = 8;
    std::array<Page *, PageStackSize> _pageStack;
    int _pageStackPos = -1;
};
