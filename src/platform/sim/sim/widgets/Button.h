#pragma once

#include "../Widget.h"

namespace sim {

class Button : public Widget {
public:
    typedef std::shared_ptr<Button> Ptr;

    Button(const Vector2i &pos, const Vector2i &size, SDL_Keycode keycode = -1) :
        Widget(pos, size),
        _keycode(keycode)
    {}

    bool pressed() const { return _pressed; }

    void setCallback(std::function<void(bool)> callback) {
        _callback = callback;
    }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(_hovered ? 0.75f : 0.5f, 1.f));
        renderer.drawRect(_pos, _size);
        if (_pressed) {
            renderer.setColor(Color(1.f, 1.f));
            renderer.fillRect(_pos + Vector2i(4, 4), _size - Vector2i(8, 8));
        }
    }

    virtual void onKeyDown(KeyEvent &e) override {
        if (!_pressed && e.keycode() == _keycode) {
            setPressed(true);
            e.consume();
        }
    }

    virtual void onKeyUp(KeyEvent &e) override {
        if (_pressed && e.keycode() == _keycode) {
            setPressed(false);
            e.consume();
        }
    }

    virtual void onMouseMove(MouseMoveEvent &e) override {
        _hovered = isInside(e.pos());
    }

    virtual void onMouseDown(MouseButtonEvent &e) override {
        if (!_pressed && e.button() == MouseButtonEvent::Left && isInside(e.pos())) {
            setPressed(true);
            e.consume();
        }
    }

    virtual void onMouseUp(MouseButtonEvent &e) override {
        if (_pressed && e.button() == MouseButtonEvent::Left) {
            setPressed(false);
        }
    }

private:
    void setPressed(bool pressed) {
        if (pressed != _pressed) {
            _pressed = pressed;
            if (_callback) {
                _callback(_pressed);
            }
        }
    }

    SDL_Keycode _keycode;
    bool _pressed = false;
    bool _hovered = false;
    std::function<void(bool)> _callback;
};

} // namespace sim
