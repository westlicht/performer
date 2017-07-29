#pragma once

#include "Common.h"

namespace sim {

class Event {
public:
    bool consumed() const { return _consumed; }
    void consume() { _consumed = true; }

private:
    bool _consumed = false;
};

class KeyEvent : public Event {
public:
    KeyEvent(SDL_Scancode scancode, SDL_Keycode keycode, uint16_t modifiers) :
        _scancode(scancode),
        _keycode(keycode),
        _modifiers(modifiers)
    {}

    SDL_Scancode scancode() const { return _scancode; }
    SDL_Keycode keycode() const { return _keycode; }
    uint16_t modifiers() const { return _modifiers; }

    static KeyEvent fromSDL(const SDL_KeyboardEvent &e) {
        return KeyEvent(e.keysym.scancode, e.keysym.sym, e.keysym.mod);
    }

private:
    SDL_Scancode _scancode;
    SDL_Keycode _keycode;
    uint16_t _modifiers;
};

class MouseMoveEvent : public Event {
public:
    MouseMoveEvent(const Vector2i &pos, const Vector2i &rel) :
        _pos(pos),
        _rel(rel)
    {}

    const Vector2i &pos() const { return _pos; }
    const Vector2i &rel() const { return _rel; }

    static MouseMoveEvent fromSDL(const SDL_MouseMotionEvent &e) {
        return MouseMoveEvent(Vector2i(e.x, e.y), Vector2i(e.xrel, e.yrel));
    }

private:
    Vector2i _pos;
    Vector2i _rel;
};

class MouseButtonEvent : public Event {
public:
    enum Button {
        Left,
        Middle,
        Right,
        Unknown,
    };

    MouseButtonEvent(const Vector2i &pos, Button button, int clicks) :
        _pos(pos),
        _button(button),
        _clicks(clicks)
    {}

    const Vector2i &pos() const { return _pos; }
    Button button() const { return _button; }
    int clicks() const { return _clicks; }

    static MouseButtonEvent fromSDL(const SDL_MouseButtonEvent &e) {
        auto button = [] (uint8_t button) {
            switch (button) {
            case SDL_BUTTON_LEFT: return Left;
            case SDL_BUTTON_MIDDLE: return Middle;
            case SDL_BUTTON_RIGHT: return Right;
            default: return Unknown;
            }
        };
        return MouseButtonEvent(Vector2i(e.x, e.y), button(e.button), e.clicks);
    }

private:
    Vector2i _pos;
    Button _button;
    int _clicks;
};

class MouseWheelEvent : public Event {
public:
    MouseWheelEvent(const Vector2i &scroll) :
        _scroll(scroll)
    {}

    const Vector2i &scroll() const { return _scroll; }

    static MouseWheelEvent fromSDL(const SDL_MouseWheelEvent &e) {
        return MouseWheelEvent(Vector2i(e.x, e.y));
    }

private:
    Vector2i _scroll;
};

} // namespace sim
