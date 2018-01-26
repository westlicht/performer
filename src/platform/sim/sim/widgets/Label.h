#pragma once

#include "../Widget.h"
#include "../Text.h"

#include <string>
#include <memory>

namespace sim {

class Label : public Widget {
public:
    typedef std::shared_ptr<Label> Ptr;

    Label(const Vector2i &pos, const Vector2i &size, const std::string &text, const Font &font, const Color &color = Color(1.f, 1.f)) :
        Widget(pos, size),
        _text(text),
        _font(font),
        _color(color)
    {
    }

    const std::string &text() const { return _text; }
    void setText(const std::string &text) { _text = text; _dirty = true; }

    const Font &font() const { return _font; }
    void setFont(const Font &font) { _font = font; _dirty = true; }

    const Color &color() const { return _color; }
    void setColor(const Color &color) { _color = color; _dirty = true; }

    virtual void update() override {
    }

    virtual void render(Renderer &renderer) override {
        if (_dirty) {
            _texture.reset(_text.empty() ? nullptr : new Text(renderer, _text, _font, _color));
            _dirty = false;
        }

        if (_texture) {
            auto pos = _pos + (_size - _texture->size()) / 2;
            renderer.drawText(pos, *_texture);
        }
    }

private:
    std::string _text;
    Font _font;
    Color _color;

    std::unique_ptr<Text> _texture;
    bool _dirty = true;
};

} // namespace sim
