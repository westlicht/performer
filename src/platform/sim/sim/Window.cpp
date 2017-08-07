#include "Window.h"

namespace sim {

Window::Window(const std::string &title, const Vector2i &size) :
    _window(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x(), size.y(), 0),
    _renderer(_window)
{
    _eventHandler.quit = [this] (const SDL_QuitEvent &) {
        _terminate = true;
    };
    _eventHandler.keyDown = [this] (const SDL_KeyboardEvent &e) {
        handleEvent(KeyEvent::fromSDL(e), [] (Widget &widget, KeyEvent &e) { widget.onKeyDown(e); } );
    };
    _eventHandler.keyUp = [this] (const SDL_KeyboardEvent &e) {
        handleEvent(KeyEvent::fromSDL(e), [] (Widget &widget, KeyEvent &e) { widget.onKeyUp(e); } );
    };
    _eventHandler.mouseMotion = [this] (const SDL_MouseMotionEvent &e) {
        handleEvent(MouseMoveEvent::fromSDL(e), [] (Widget &widget, MouseMoveEvent &e) { widget.onMouseMove(e); } );
    };
    _eventHandler.mouseButtonDown = [this] (const SDL_MouseButtonEvent &e) {
        handleEvent(MouseButtonEvent::fromSDL(e), [] (Widget &widget, MouseButtonEvent &e) { widget.onMouseDown(e); } );
    };
    _eventHandler.mouseButtonUp = [this] (const SDL_MouseButtonEvent &e) {
        handleEvent(MouseButtonEvent::fromSDL(e), [] (Widget &widget, MouseButtonEvent &e) { widget.onMouseUp(e); } );
    };
    _eventHandler.mouseWheel = [this] (const SDL_MouseWheelEvent &e) {
        handleEvent(MouseWheelEvent::fromSDL(e), [] (Widget &widget, MouseWheelEvent &e) { widget.onMouseWheel(e); } );
    };
}

void Window::update() {
    while (_eventHandler.poll()) {}

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
