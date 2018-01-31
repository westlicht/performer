#include "Page.h"

#include "PageManager.h"

Page::Page(PageManager &manager) :
    _manager(manager)
{
}

void Page::show() {
    _manager.push(this);
}

void Page::close() {
    _manager.pop();
}

bool Page::isTop() {
    return _manager.top() == this;
}

void Page::dispatchEvent(Event &event) {
    switch (event.type()) {
    case Event::KeyDown:
        keyDown(event.as<KeyEvent>());
        break;
    case Event::KeyUp:
        keyUp(event.as<KeyEvent>());
        break;
    case Event::Encoder:
        encoder(event.as<EncoderEvent>());
        break;
    }
}
