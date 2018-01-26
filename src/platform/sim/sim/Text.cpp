#include "Text.h"

#include "Renderer.h"

#include <SDL_ttf.h>

#include <map>

namespace sim {

static TTF_Font *getFont(const Font &font) {
    static bool initialized;
    static std::map<Font, TTF_Font *> fonts;

    if (!initialized) {
        TTF_Init();
        initialized = true;
    }

    if (fonts.find(font) == fonts.end()) {
        auto filename = std::string("assets/fonts/") + font.first + ".ttf";
        fonts[font] = TTF_OpenFont(filename.c_str(), font.second);
    }

    return fonts[font];
}

Text::Text(Renderer &renderer, const std::string &text, const Font &font, const Color &color) :
    _surface(TTF_RenderText_Blended(getFont(font), text.c_str(), color.sdl())),
    _texture(&renderer.sdl(), &_surface)
{
    SDL_Rect clipRect;
    _surface.getClipRect(&clipRect);
    _size = Vector2i(clipRect.w, clipRect.h);
}

} // namespace sim

