#pragma once

#include "../Widget.h"

#include "nanovg.h"

#include <cstdint>

namespace sim {

class Display : public Widget {
public:
    typedef std::shared_ptr<Display> Ptr;

    Display(const Vector2f &pos, const Vector2f &size, const Vector2i &resolution, const Color &color = Color(0.8f, 0.9f, 0.f, 1.f)) :
        Widget(pos, size),
        _resolution(resolution),
        _color(color),
        _frameBuffer(new uint32_t[resolution.prod()])
    {
    }

    const Vector2i &resolution() { return _resolution; }

    const Color &color() const { return _color; }
          Color &color()       { return _color; }

    void draw(const uint8_t *frameBuffer) {
        const uint8_t *src = frameBuffer;
        uint32_t *dst = _frameBuffer.get();
        for (int i = 0; i < _resolution.prod(); ++i) {
            float s = std::min(uint8_t(15), *src++) * (1.f / 15.f);
            *dst++ = Color(s * _color.r(), s * _color.g(), s * _color.b(), 1.f).rgba();
        }
        _frameBufferDirty = true;
    }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        auto nvg = renderer.nvg();
        const uint8_t *frameBuffer = reinterpret_cast<uint8_t *>(_frameBuffer.get());

        // update texture
        if (_image == -1) {
            _image = nvgCreateImageRGBA(nvg, _resolution.x(), _resolution.y(), 0 /*NVG_IMAGE_NEAREST*/, frameBuffer);
            _pattern = nvgImagePattern(nvg, _pos.x(), _pos.y(), _size.x(), _size.y(), 0.f, _image, 1.f);;
        } else {
            if (_frameBufferDirty) {
                nvgUpdateImage(nvg, _image, frameBuffer);
                _frameBufferDirty = false;
            }
        }

		nvgBeginPath(nvg);
        nvgRect(nvg, _pos.x(), _pos.y(), _size.x(), _size.y());
        nvgFillPaint(nvg, _pattern);
        nvgFill(nvg);

        renderer.setColor(Color(0.5f, 1.f));
        renderer.drawRect(_pos, _size);
    }

private:
    Vector2i _resolution;
    Color _color;
    std::unique_ptr<uint32_t[]> _frameBuffer;
    bool _frameBufferDirty = true;
    int _image = -1;
    NVGpaint _pattern;
};

} // namespace sim
