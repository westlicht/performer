#pragma once

#include "Common.h"
#include "Widget.h"
#include "Renderer.h"

namespace sim {

class Window {
public:
    Window(const std::string &title, const Vector2i &size);

    bool terminate() const { return _terminate; }

    void update();
    void render();

    template<typename T, typename... Args>
    std::shared_ptr<T> createWidget(Args... args) {
        std::shared_ptr<T> widget = std::make_shared<T>(args...);
        addWidget(widget);
        return widget;
    }

    void addWidget(Widget::Ptr widget);
    void removeWidget(Widget::Ptr widget);

    sdl::Window &sdl() { return _window; }

private:
    template<typename Event, typename Handler>
    void handleEvent(Event event, Handler handler) {
        for (const auto &widget : _widgets) {
            handler(*widget, event);
            if (event.consumed()) {
                break;
            }
        }
    }

    sdl::Window _window;
    sdl::EventHandler _eventHandler;
    Renderer _renderer;

    std::vector<Widget::Ptr> _widgets;

    bool _terminate = false;
};

} // namespace sim
