#include "Renderer.h"

#ifdef __APPLE__
    #include <OpenGL/gl3.h>
#endif

#if defined(__linux__) || defined(__EMSCRIPTEN__)
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
#endif

#ifdef __EMSCRIPTEN__
    #define NANOVG_GLES2_IMPLEMENTATION
#else
    #define NANOVG_GL3_IMPLEMENTATION
#endif

#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"

namespace sim {

Renderer::Renderer(sdl::Window &window) :
    _window(window)
{

    _context = _window.glCreateContext();
    _window.glMakeCurrent(_context);

#ifdef __EMSCRIPTEN__
    _nvg = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#else
    _nvg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
    if (!_nvg) {
        std::cerr << "Failed to create NanoVG context!" << std::endl;
    }

    _font = nvgCreateFont(_nvg, "monospace", "assets/fonts/inconsolata.ttf");
}

void Renderer::clear() {
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);

    int width, height;
    _window.getSize(&width, &height);

    nvgBeginFrame(_nvg, width, height, 1.f);
}

void Renderer::setColor(const Color &color) {
    nvgFillColor(_nvg, nvgRGBA(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255));
    nvgStrokeColor(_nvg, nvgRGBA(color.r() * 255, color.g() * 255, color.b() * 255, color.a() * 255));
}

void Renderer::drawRect(const Vector2i &pos, const Vector2i &size) {
    nvgBeginPath(_nvg);
    nvgRect(_nvg, pos.x(), pos.y(), size.x(), size.y());
    nvgStroke(_nvg);
}

void Renderer::fillRect(const Vector2i &pos, const Vector2i &size) {
    nvgBeginPath(_nvg);
    nvgRect(_nvg, pos.x(), pos.y(), size.x(), size.y());
    nvgFill(_nvg);
}

void Renderer::drawEllipse(const Vector2i &pos, const Vector2i &size) {
    nvgBeginPath(_nvg);
    nvgEllipse(_nvg, pos.x() + size.x() / 2, pos.y() + size.y() / 2, size.x() / 2, size.y() / 2);
    nvgStroke(_nvg);
}

void Renderer::fillEllipse(const Vector2i &pos, const Vector2i &size) {
    nvgBeginPath(_nvg);
    nvgEllipse(_nvg, pos.x() + size.x() / 2, pos.y() + size.y() / 2, size.x() / 2, size.y() / 2);
    nvgFill(_nvg);
}

void Renderer::drawLine(const Vector2i &p1, const Vector2i &p2) {
    nvgBeginPath(_nvg);
    nvgMoveTo(_nvg, p1.x(), p1.y());
    nvgLineTo(_nvg, p2.x(), p2.y());
    nvgStroke(_nvg);
}

void Renderer::drawArc(const Vector2i &pos, const Vector2i &size, float rotation) {
    nvgBeginPath(_nvg);
    nvgArc(_nvg, pos.x() + size.x() / 2, pos.y() + size.y() / 2, size.x() / 2, -0.5f * M_PI, (rotation - 0.5f) * M_PI, rotation > 0.f ? NVG_CW : NVG_CCW);
    nvgStroke(_nvg);
}

void Renderer::drawText(const Vector2i &pos, const std::string &text) {
    float bounds[4];
    nvgFontFaceId(_nvg, _font);
    nvgFontSize(_nvg, 12.f);
    nvgTextBounds(_nvg, 0.f, 0.f, text.c_str(), nullptr, bounds);
    nvgText(_nvg, pos.x() - bounds[2] / 2, pos.y() - bounds[3] / 2, text.c_str(), nullptr);
}

void Renderer::present() {
    nvgEndFrame(_nvg);

    _window.glSwap();
}

} // namespace sim
