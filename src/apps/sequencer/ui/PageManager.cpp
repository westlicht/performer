#include "PageManager.h"

#include "core/Debug.h"

PageManager::PageManager(Pages &pages) :
    _pages(pages)
{}

Page *PageManager::top() {
    ASSERT(_pageStackPos >= 0, "accessing empty page stack");
    return _pageStack[_pageStackPos];
}

void PageManager::push(Page *page) {
    ASSERT(_pageStackPos < PageStackSize - 1, "page stack overflow");
    _pageStack[++_pageStackPos] = page;
    page->enter();
}

void PageManager::pop() {
    ASSERT(_pageStackPos > 0, "page stack underflow");
    top()->exit();
    --_pageStackPos;
}

void PageManager::reset(Page *page) {
    _pageStackPos = -1;
    push(page);
}

void PageManager::draw(Canvas &canvas) {
    // draw bottom to top
    for (int i = 0; i <= _pageStackPos; ++i) {
        _pageStack[i]->draw(canvas);
    }
}

void PageManager::updateLeds(Leds &leds) {
    // update bottom to top
    for (int i = 0; i <= _pageStackPos; ++i) {
        _pageStack[i]->updateLeds(leds);
    }
}

void PageManager::dispatchEvent(Event &event) {
    // handle top to bottom
    for (int i = _pageStackPos; i >= 0; --i) {
        _pageStack[i]->dispatchEvent(event);
        if (event.consumed()) {
            break;
        }
    }
}
