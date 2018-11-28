#pragma once

#include "../Widget.h"

namespace sim {

class Button : public Widget {
public:
    typedef std::shared_ptr<Button> Ptr;

    enum Shape {
        Rectangle,
        Ellipse,
    };

    Button(const Vector2f &pos, const Vector2f &size, Shape shape = Rectangle, SDL_Keycode keycode = -1) :
        Widget(pos, size),
        _shape(shape),
        _keycode(keycode)
    {}

    // bool pressed() const { return _pressed; }

    bool state() const { return _state; }
    void setState(bool state) { _state = state; }

    void setCallback(std::function<void(bool)> callback) {
        _callback = callback;
    }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(_hovered ? 0.75f : 0.5f, 1.f));
        switch (_shape) {
        case Rectangle: renderer.drawRect(_pos, _size);     break;
        case Ellipse:   renderer.drawEllipse(_pos, _size);  break;
        }

        if (_state) {
            renderer.setColor(Color(1.f, 1.f));
            switch (_shape) {
            case Rectangle: renderer.fillRect(_pos + Vector2f(1, 1), _size - Vector2f(2, 2));       break;
            case Ellipse:   renderer.fillEllipse(_pos + Vector2f(1, 1), _size - Vector2f(2, 2));    break;
            }
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

    Shape _shape;
    SDL_Keycode _keycode;
    bool _pressed = false;
    bool _hovered = false;
    bool _state = false;
    std::function<void(bool)> _callback;
};

} // namespace sim
