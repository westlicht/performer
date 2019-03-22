#pragma once

#include "core/Debug.h"

#include <array>
#include <functional>

#include <cstdint>
#include <cstddef>

template<typename Event, size_t MaxObservers>
class Observable {
public:
    typedef std::function<void(Event event)> Handler;

    void watch(Handler handler) {
        ASSERT(_observerCount < MaxObservers, "too many observers");
        auto &observer = _observers[_observerCount++];
        observer.handler = handler;
    }

    void notify(Event event) {
        for (size_t i = 0; i < _observerCount; ++i) {
            _observers[i].handler(event);
        }
    }

private:
    struct Observer {
        Handler handler;
    };

    std::array<Observer, MaxObservers> _observers;
    size_t _observerCount = 0;
};
