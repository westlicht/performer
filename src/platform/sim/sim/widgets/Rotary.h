#pragma once

#include "../Widget.h"

namespace sim {

class Rotary : public Widget {
public:
    typedef std::shared_ptr<Rotary> Ptr;

    Rotary(const Vector2i &pos, const Vector2i &size) :
        Widget(pos, size)
    {}

    float value() const { return _value; }
    void setValue(float value) { _value = std::max(0.f, std::min(1.f, value)); }

    bool pressed() const { return _pressed; }

    void setButtonCallback(std::function<void(bool)> callback) {
        _buttonCallback = callback;
    }

    void setValueCallback(std::function<void(float)> callback) {
        _valueCallback = callback;
    }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(_hovered ? 0.75f : 0.5f, 1.f));

        if (_pressed) {
            renderer.setColor(Color(1.f, 1.f));
        }

        Vector2f center = toFloat(_pos) + 0.5f * toFloat(_size);
        float radius = 0.5f * std::max(_size.x(), _size.y()) - 2.f;

        auto pointOnCircle = [&] (float theta) -> Vector2i {
            return toInt(center + radius * Vector2f(std::sin(theta), -std::cos(theta)));
        };

        const int Segments = 64;
        for (int i = 0; i < Segments; ++i) {
            auto theta1 = float(i) / Segments * TWO_PI;
            auto theta2 = float(i + 1) / Segments * TWO_PI;
            renderer.drawLine(pointOnCircle(theta1), pointOnCircle(theta2));
        }

        renderer.drawLine(toInt(center), pointOnCircle(-PI * 0.75f + _value * TWO_PI * 0.75f));
    }

    virtual void onMouseMove(MouseMoveEvent &e) override {
        _hovered = isInside(e.pos());
        if (_pressed) {
            int delta = e.pos().x() - _lastPos.x();
            updateValue(_value + delta / 100.f);
            _lastPos = e.pos();
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

private:
    void setPressed(bool pressed) {
        if (pressed != _pressed) {
            _pressed = pressed;
            if (_buttonCallback) {
                _buttonCallback(_pressed);
            }
        }
    }

    void updateValue(float value) {
        value = std::max(0.f, std::min(1.f, value));
        if (value != _value) {
            _value = value;
            if (_valueCallback) {
                _valueCallback(value);
            }
        }
    }

    bool _pressed = false;
    bool _hovered = false;
    float _value = 0.5f;
    Vector2i _lastPos;

    std::function<void(bool)> _buttonCallback;
    std::function<void(float)> _valueCallback;
};

} // namespace sim
