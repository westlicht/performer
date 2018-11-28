#pragma once

#include "../Widget.h"

namespace sim {

class Encoder : public Widget {
public:
    typedef std::shared_ptr<Encoder> Ptr;

    Encoder(const Vector2f &pos, const Vector2f &size, SDL_Keycode keycode = -1) :
        Widget(pos, size),
        _keycode(keycode)
    {}

    bool pressed() const { return _pressed; }

    void setButtonCallback(std::function<void(bool)> callback) {
        _buttonCallback = callback;
    }

    void setValueCallback(std::function<void(int)> callback) {
        _valueCallback = callback;
    }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(_hovered ? 0.75f : 0.5f, 1.f));

        if (_pressed) {
            renderer.setColor(Color(1.f, 1.f));
        }

        Vector2f center = _pos + 0.5f * _size;
        float radius = 0.5f * std::max(_size.x(), _size.y()) - 2.f;

        auto pointOnCircle = [&] (float theta) -> Vector2f {
            return center + radius * Vector2f(std::sin(theta), -std::cos(theta));
        };

        renderer.drawEllipse(_pos, _size);
        renderer.drawLine(center, pointOnCircle(_value * TWO_PI / Ticks));
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
        if (_pressed) {
            int delta = e.pos().x() - _lastPos.x();
            if (delta / 10 != 0) {
                setValue(_value + delta / 10);
                _lastPos = e.pos();
            }
        }
    }

    virtual void onMouseDown(MouseButtonEvent &e) override {
        if (!_pressed && e.button() == MouseButtonEvent::Left && isInside(e.pos())) {
            SDL_CaptureMouse(SDL_TRUE);
            _lastPos = e.pos();
            setPressed(true);
            e.consume();
        }
    }

    virtual void onMouseUp(MouseButtonEvent &e) override {
        if (_pressed && e.button() == MouseButtonEvent::Left) {
            SDL_CaptureMouse(SDL_FALSE);
            setPressed(false);
            e.consume();
        }
    }

    virtual void onMouseWheel(MouseWheelEvent &e) override {
        _deltaValue += e.scroll().y();
        setValue(_value + _deltaValue / ScrollDivider);
        _deltaValue %= ScrollDivider;
    }

private:
    void setPressed(bool pressed) {
        if (pressed != _pressed) {
            _pressed = pressed;
            if (_buttonCallback) {
                _buttonCallback(_pressed);
            }
        }
    }

    void setValue(int value) {
        if (value != _value) {
            int delta = value - _value;
            _value = value;
            if (_valueCallback) {
                _valueCallback(delta);
            }
        }
    }

    static const int ScrollDivider = 4;
    static const int Ticks = 24;

    SDL_Keycode _keycode;
    bool _pressed = false;
    bool _hovered = false;
    int _value = 0;
    int _deltaValue = 0;
    Vector2i _lastPos;

    std::function<void(bool)> _buttonCallback;
    std::function<void(int)> _valueCallback;
};

} // namespace sim
