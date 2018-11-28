#pragma once

#include "../Widget.h"

namespace sim {

class Panel : public Widget {
public:
    typedef std::shared_ptr<Panel> Ptr;

    Panel(const Vector2f &pos, const Vector2f &size, const Color &color) :
        Widget(pos, size),
        _color(color)
    {}

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        renderer.setColor(_color);
        renderer.fillRect(_pos, _size);
    }

private:
    Color _color;
};

} // namespace sim
