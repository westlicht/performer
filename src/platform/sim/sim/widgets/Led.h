#pragma once

#include "../Widget.h"

namespace sim {

class Led : public Widget {
public:
    typedef std::shared_ptr<Led> Ptr;

    Led(const Vector2i &pos, const Vector2i &size, const Color &color = Color(0.f, 1.f)) :
        Widget(pos, size),
        _color(color)
    {}

    const Color &color() const { return _color; }
          Color &color()       { return _color; }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawRect(_pos, _size);
        renderer.setColor(_color);
        renderer.fillRect(_pos + Vector2i(1, 1), _size - Vector2i(2, 2));
    }

private:
    Color _color;
};

} // namespace sim
