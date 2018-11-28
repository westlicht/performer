#pragma once

#include "Common.h"
#include "Widget.h"
#include "Renderer.h"

namespace sim {

class Window {
public:
    typedef std::shared_ptr<Window> Ptr;

    Window(const std::string &title, const Vector2i &size);
    ~Window();

    void close() { _terminate = true; }

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

    SDL_Window *_window;
    Renderer _renderer;
    std::vector<Widget::Ptr> _widgets;
    bool _terminate = false;
};

} // namespace sim
