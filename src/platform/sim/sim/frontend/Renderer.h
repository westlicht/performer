#pragma once

#include "Common.h"

#include <string>

struct NVGcontext;

namespace sim {

class Renderer {
public:
    Renderer(sdl::Window &window);

    NVGcontext *nvg() const { return _nvg; }

    void clear() ;
    void setColor(const Color &color);
    void drawRect(const Vector2i &pos, const Vector2i &size);
    void fillRect(const Vector2i &pos, const Vector2i &size);
    void drawEllipse(const Vector2i &pos, const Vector2i &size);
    void fillEllipse(const Vector2i &pos, const Vector2i &size);
    void drawLine(const Vector2i &p1, const Vector2i &p2);
    void drawArc(const Vector2i &pos, const Vector2i &size, float rotation);

    void drawText(const Vector2i &pos, const std::string &text);

    void present();

private:
    sdl::Window &_window;
    SDL_GLContext _context;
    NVGcontext *_nvg;
    int _font;
};

} // namespace sim
