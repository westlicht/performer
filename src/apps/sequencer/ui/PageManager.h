#pragma once

#include "Page.h"

#include "Event.h"
#include "Leds.h"

#include "core/gfx/Canvas.h"

#include <array>

struct Pages;

class PageManager {
public:
    PageManager(Pages &pages);

    Pages &pages() { return _pages; }

    Page *top();
    void push(Page *page);
    void pop();
    void reset(Page *page);

    void draw(Canvas &canvas);
    void updateLeds(Leds &leds);
    void dispatchEvent(Event &event);

private:
    Pages &_pages;
    static const int PageStackSize = 8;
    std::array<Page *, PageStackSize> _pageStack;
    int _pageStackPos = -1;
};
