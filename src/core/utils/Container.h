#pragma once

#include <cstdint>

template<typename... Ts>
struct maxsizeof {
    static constexpr size_t value = 0;
};

template<typename T, typename... Ts>
struct maxsizeof<T, Ts...> {
    static constexpr size_t value = sizeof(T) > maxsizeof<Ts...>::value ? sizeof(T) : maxsizeof<Ts...>::value;
};

template<typename... Ts>
class Container {
public:
    static constexpr size_t Size = maxsizeof<Ts...>::value;

    template<typename U, typename... Args>
    U *create(Args&&... args) {
        return new(_data) U(args...);
    }

    template<typename U>
    void destroy(U *object) {
        if (object) {
            object->~U();
        }
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

    template<typename U>
    U &as() {
        return *reinterpret_cast<U *>(_data);
    }

    template<typename U>
    const U &as() const {
        return *reinterpret_cast<const U *>(_data);
    }

#pragma GCC diagnostic pop

private:
    // memory aligned to system pointer size
    uintptr_t _data[(Size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)];
};
