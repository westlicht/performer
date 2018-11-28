#pragma once

#include "../Widget.h"

namespace sim {

class Led : public Widget {
public:
    typedef std::shared_ptr<Led> Ptr;

    Led(const Vector2f &pos, const Vector2f &size, const Color &color = Color(0.f, 1.f)) :
        Widget(pos, size),
        _color(color)
    {}

    const Color &color() const { return _color; }
          Color &color()       { return _color; }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawEllipse(_pos, _size);
        renderer.setColor(_color);
        renderer.fillEllipse(_pos + Vector2f(1.f, 1.f), _size - Vector2f(2.f, 2.f));
    }

private:
    Color _color;
};

} // namespace sim
