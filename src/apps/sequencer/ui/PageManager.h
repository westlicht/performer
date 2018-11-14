#pragma once

#include "Page.h"

#include "Event.h"
#include "Leds.h"

#include "core/gfx/Canvas.h"

#include <array>
#include <functional>

struct Pages;

class PageManager {
public:
    // called before entering a page
    typedef std::function<void(Page *)> PageSwitchHandler;

    PageManager(Pages &pages);

    Pages &pages() { return _pages; }

    Page *top() const;
    void push(Page *page);
    void pop();

    int stackSize() const { return _pageStackPos + 1; }

    void reset(Page *page);
    void replace(int index, Page *page);

    void draw(Canvas &canvas);
    void updateLeds(Leds &leds);

    int fps() const;

    void dispatchEvent(Event &event);

    void setPageSwitchHandler(PageSwitchHandler pageSwitchHandler) {
        _pageSwitchHandler = pageSwitchHandler;
    }

private:
    void notifyPageSwitch(Page *page) {
        if (_pageSwitchHandler) {
            _pageSwitchHandler(page);
        }
    }

    Pages &_pages;
    static const int PageStackSize = 8;
    std::array<Page *, PageStackSize> _pageStack;
    int _pageStackPos = -1;
    PageSwitchHandler _pageSwitchHandler;
};
