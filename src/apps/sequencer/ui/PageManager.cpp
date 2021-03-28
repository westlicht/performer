#include "PageManager.h"

#include "core/Debug.h"

PageManager::PageManager(Pages &pages) :
    _pages(pages)
{}

Page *PageManager::top() const {
    ASSERT(_pageStackPos >= 0, "accessing empty page stack");
    return _pageStack[_pageStackPos];
}

void PageManager::push(Page *page) {
    ASSERT(_pageStackPos < PageStackSize - 1, "page stack overflow");
    _pageStack[++_pageStackPos] = page;
    page->enter();

    notifyPageSwitch(page);
}

void PageManager::pop() {
    ASSERT(_pageStackPos > 0, "page stack underflow");
    top()->exit();
    --_pageStackPos;

    if (_pageStackPos >= 0) {
        notifyPageSwitch(top());
    }
}

void PageManager::reset(Page *page) {
    while (_pageStackPos >= 0) {
        _pageStack[_pageStackPos--]->exit();
    }
    push(page);
}

void PageManager::replace(int index, Page *page) {
    ASSERT(index >= 0 && index <= _pageStackPos, "invalid page index");

    auto &pagePtr = _pageStack[index];
    pagePtr->exit();
    pagePtr = page;
    pagePtr->enter();

    if (index == _pageStackPos) {
        notifyPageSwitch(page);
    }
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

int PageManager::fps() const {
    int fps = 100;
    for (int i = 0; i <= _pageStackPos; ++i) {
        fps = std::min(fps, _pageStack[i]->fps());
    }
    return fps;
}


void PageManager::dispatchEvent(Event &event) {
    // handle modal page
    if (top()->isModal() && !event.consumed()) {
        top()->dispatchEvent(event);
        return;
    }

    // handle top to bottom
    for (int i = _pageStackPos; i >= 0; --i) {
        if (event.consumed()) {
            break;
        }
        _pageStack[i]->dispatchEvent(event);
    }
}
