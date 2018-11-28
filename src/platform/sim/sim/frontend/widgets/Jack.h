#pragma once

#include "../Widget.h"

namespace sim {

class Jack : public Widget {
public:
    typedef std::shared_ptr<Jack> Ptr;

    Jack(const Vector2f &pos, const Vector2f &size, const Color &color = Color(0.f, 1.f)) :
        Widget(pos, size),
        _color(color)
    {}

    const Color &color() const { return _color; }
          Color &color()       { return _color; }

    void setState(bool state) {
        _signal = State;
        _state = state;
    }

    void setValue(float value, float min, float max) {
        _signal = Value;
        _value = value;
        _min = min;
        _max = max;
    }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawEllipse(_pos, _size);
        switch (_signal) {
        case None:
            break;
        case State:
            if (_state) {
                renderer.setColor(Color(1.f, 1.f));
                renderer.drawEllipse(_pos + Vector2f(1.f, 1.f), _size - Vector2f(2.f, 2.f));
            }
            break;
        case Value:
            float rotation = ((_value - _min) / (_max - _min)) * 2.f - 1.f;
            renderer.setColor(Color(1.f, 1.f));
            renderer.drawArc(_pos + Vector2f(1.f, 1.f), _size - Vector2f(2.f, 2.f), rotation);
            break;
        }
    }

private:
    Color _color;
    enum Signal {
        None,
        State,
        Value,
    };
    Signal _signal = None;
    bool _state;
    float _value;
    float _min;
    float _max;
};

} // namespace sim
