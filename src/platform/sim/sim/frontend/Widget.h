#pragma once

#include "Common.h"
#include "Renderer.h"
#include "Event.h"

namespace sim {

class Widget {
public:
    typedef std::shared_ptr<Widget> Ptr;

    Widget(const Vector2f &pos, const Vector2f &size) :
        _pos(pos),
        _size(size)
    {}

    virtual ~Widget() {}

    const Vector2f &pos() const { return _pos; }
          Vector2f &pos()       { return _pos; }

    const Vector2f &size() const { return _size; }
          Vector2f &size()       { return _size; }

    bool isInside(const Vector2f &pos) const {
        return (
            pos.x() >= _pos.x() &&
            pos.x() <= _pos.x() + _size.x() &&
            pos.y() >= _pos.y() &&
            pos.y() <= _pos.y() + _size.y()
        );
    }

    bool isInside(const Vector2i &pos) const {
        return (
            pos.x() >= _pos.x() &&
            pos.x() <= _pos.x() + _size.x() &&
            pos.y() >= _pos.y() &&
            pos.y() <= _pos.y() + _size.y()
        );
    }

    virtual void update() = 0;
    virtual void render(Renderer &renderer) = 0;

    virtual void onKeyDown(KeyEvent &e) {}
    virtual void onKeyUp(KeyEvent &e) {}
    virtual void onMouseMove(MouseMoveEvent &e) {}
    virtual void onMouseDown(MouseButtonEvent &e) {}
    virtual void onMouseUp(MouseButtonEvent &e) {}
    virtual void onMouseWheel(MouseWheelEvent &e) {}

protected:
    Vector2f _pos;
    Vector2f _size;
};

} // namespace sim
