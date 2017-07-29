#pragma once

#include "Common.h"
#include "Renderer.h"

namespace sim {

class Renderer {
public:
    Renderer(sdl::Window &window) :
        _renderer(&window, -1, SDL_RENDERER_ACCELERATED)
    {}

    void clear() { _renderer.clear(); }

    void setColor(const Color &color) {
        _renderer.setDrawColor(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255);
    }

    void drawRect(const Vector2i &pos, const Vector2i &size) {
        SDL_Rect rect = { pos.x(), pos.y(), size.x(), size.y() };
        _renderer.drawRect(&rect);
    }

    void fillRect(const Vector2i &pos, const Vector2i &size) {
        SDL_Rect rect = { pos.x(), pos.y(), size.x(), size.y() };
        _renderer.fillRect(&rect);
    }

    void drawLine(const Vector2i &p1, const Vector2i &p2) {
        _renderer.drawLine(p1.x(), p1.y(), p2.x(), p2.y());
    }

    void present() { _renderer.present(); }

    sdl::Renderer &sdl() { return _renderer; }

private:
    sdl::Renderer _renderer;
};

} // namespace sim
