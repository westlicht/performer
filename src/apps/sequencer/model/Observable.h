
#include <array>
#include <functional>

#include <cstdint>
#include <cstddef>

template<size_t MaxObservers, typename Id = uint8_t>
class Observable {
public:
    template<typename T>
    void watch(Id id, std::function<void(const T &, const T &)> handler) {
        if (_observerCount == MaxObservers) {
            return;
        }
        auto &observer = _observers[_observerCount++];
        observer.id = id;
        observer.handler = [handler] (const void *newValue, const void *oldValue) {
            handler(*static_cast<const T *>(newValue), *static_cast<const T *>(oldValue));
        };
    }

protected:
    template<typename T>
    void notify(Id id, const T &newValue, const T &oldValue) {
        for (size_t i = 0; i < _observerCount; ++i) {
            const auto &observer = _observers[i];
            if (observer.id == id) {
                observer.handler(&newValue, &oldValue);
            }
        }
    }

    template<typename T>
    void updateValue(Id id, T &value, const T &newValue) {
        if (newValue != value) {
            T oldValue = value;
            value = newValue;
            notify(id, newValue, oldValue);
        }
    }

private:
    struct Observer {
        Id id;
        std::function<void(const void *, const void *)> handler;
    };

    std::array<Observer, MaxObservers> _observers;
    size_t _observerCount = 0;
};


