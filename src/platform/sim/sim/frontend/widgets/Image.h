#pragma once

#include "../Widget.h"

namespace sim {

class Image : public Widget {
public:
    typedef std::shared_ptr<Image> Ptr;

    Image(const Vector2f &pos, const Vector2f &size, const std::string &filename) :
        Widget(pos, size),
        _filename(filename)
    {}

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        auto nvg = renderer.nvg();

        if (_image == -1) {
            _image = nvgCreateImage(nvg, _filename.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
            _pattern = nvgImagePattern(nvg, _pos.x(), _pos.y(), _size.x(), _size.y(), 0.f, _image, 1.f);;
        }

		nvgBeginPath(nvg);
        nvgRect(nvg, _pos.x(), _pos.y(), _size.x(), _size.y());
        nvgFillPaint(nvg, _pattern);
        nvgFill(nvg);
    }

private:
    std::string _filename;
    int _image = -1;
    NVGpaint _pattern;
};

} // namespace sim
