#pragma once

#include "Common.h"

#include <string>
#include <utility>

namespace sim {

class Renderer;

typedef std::pair<std::string, int> Font;

// class Text {
// public:
//     Text(Renderer &renderer, const std::string &text, const Font &font, const Color &color);

//     const Vector2i &size() const { return _size; }

//     sdl::Texture &texture() { return _texture; }

// private:
//     sdl::Surface _surface;
//     sdl::Texture _texture;
//     Vector2i _size;
// };

} // namespace sim
