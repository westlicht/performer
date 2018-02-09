#pragma once

#include "../Widget.h"

#include "libs/stb/stb_image_write.h"

#include <string>

namespace sim {

class Display : public Widget {
public:
    typedef std::shared_ptr<Display> Ptr;

    Display(const Vector2i &pos, const Vector2i &size, const Vector2i &resolution, const Color &color = Color(0.8f, 0.9f, 0.f, 1.f)) :
        Widget(pos, size),
        _resolution(resolution),
        _color(color),
        _surface(0, resolution.x(), resolution.y(), 32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
#else
            0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000
#endif
        )
    {
    }

    const Vector2i &resolution() { return _resolution; }

    const Color &color() const { return _color; }
          Color &color()       { return _color; }

    void draw(uint8_t *frameBuffer) {
        _surface.lock();
        uint8_t *src = frameBuffer;
        uint32_t *dst = reinterpret_cast<uint32_t *>((&_surface)->pixels);
        for (int i = 0; i < _resolution.prod(); ++i) {
            float s = std::min(uint8_t(15), *src++) * (1.f / 15.f);
            *dst++ = Color(s * _color.r(), s * _color.g(), s * _color.b(), 1.f).rgba();
        }
        _surface.unlock();
    }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        sdl::Texture texture(&renderer.sdl(), &_surface);
        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawRect(_pos, _size);
        SDL_Rect rect = { _pos.x() + 1, _pos.y() + 1, _size.x() - 2, _size.y() - 2 };
        renderer.sdl().copy(&texture, nullptr, &rect);
    }

    void screenshot(const std::string &filename) const {
        std::unique_ptr<uint8_t[]> pixels(new uint8_t[_resolution.x() * _resolution.y() * 3]);
        const uint32_t *src = reinterpret_cast<uint32_t *>((&_surface)->pixels);
        uint8_t *dst = pixels.get();
        for (int i = 0; i < _resolution.prod(); ++i) {
            uint32_t c = *src++;
            *dst++ = c & 0xff;
            *dst++ = (c >> 8) & 0xff;
            *dst++ = (c >> 16) & 0xff;
        }
        stbi_write_png(filename.c_str(), _resolution.x(), _resolution.y(), 3, pixels.get(), _resolution.x() * 3);
    }

private:
    Vector2i _resolution;
    Color _color;
    sdl::Surface _surface;
};

} // namespace sim
