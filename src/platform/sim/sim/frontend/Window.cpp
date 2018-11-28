#include "Window.h"

namespace sim {

Window::Window(const std::string &title, const Vector2i &size) :
    _window(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x(), size.y(), SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI)),
    _renderer(_window)
{
}

Window::~Window() {
    SDL_DestroyWindow(_window);
}

void Window::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            _terminate = true;
            break;
        case SDL_KEYDOWN:
            handleEvent(KeyEvent::fromSDL(event.key), [] (Widget &widget, KeyEvent &e) { widget.onKeyDown(e); } );
            break;
        case SDL_KEYUP:
            handleEvent(KeyEvent::fromSDL(event.key), [] (Widget &widget, KeyEvent &e) { widget.onKeyUp(e); } );
            break;
        case SDL_MOUSEMOTION:
            handleEvent(MouseMoveEvent::fromSDL(event.motion), [] (Widget &widget, MouseMoveEvent &e) { widget.onMouseMove(e); } );
            break;
        case SDL_MOUSEBUTTONDOWN:
            handleEvent(MouseButtonEvent::fromSDL(event.button), [] (Widget &widget, MouseButtonEvent &e) { widget.onMouseDown(e); } );
            break;
        case SDL_MOUSEBUTTONUP:
            handleEvent(MouseButtonEvent::fromSDL(event.button), [] (Widget &widget, MouseButtonEvent &e) { widget.onMouseUp(e); } );
            break;
        case SDL_MOUSEWHEEL:
            handleEvent(MouseWheelEvent::fromSDL(event.wheel), [] (Widget &widget, MouseWheelEvent &e) { widget.onMouseWheel(e); } );
            break;
        }
    }

    for (const auto &widget : _widgets) {
        widget->update();
    }
}

void Window::render() {
    _renderer.setColor(Color(0.f, 0.f, 0.f, 1.f));
    _renderer.clear();

    for (const auto &widget : _widgets) {
        widget->render(_renderer);
    }

    _renderer.present();
}

void Window::addWidget(Widget::Ptr widget) {
    _widgets.emplace_back(widget);
}

void Window::removeWidget(Widget::Ptr widget) {
    _widgets.emplace_back(widget);
}

} // namespace sim
