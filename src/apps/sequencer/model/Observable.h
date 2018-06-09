#pragma once

#include "core/Debug.h"

#include <array>
#include <functional>

#include <cstdint>
#include <cstddef>

template<typename Property, size_t MaxObservers>
class Observable {
public:
    typedef std::function<void(Property property)> Handler;

    void watch(Handler handler) {
        ASSERT(_observerCount < MaxObservers, "too many observers");
        auto &observer = _observers[_observerCount++];
        observer.handler = handler;
    }

    void notify(Property property) {
        for (size_t i = 0; i < _observerCount; ++i) {
            _observers[i].handler(property);
        }
    }

private:
    struct Observer {
        Handler handler;
    };

    std::array<Observer, MaxObservers> _observers;
    size_t _observerCount = 0;
};
