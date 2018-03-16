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
    U *create(Args... args) {
        return new(data) U(args...);
    }

    template<typename U>
    void destroy(U *object) {
        if (object) {
            object->~U();
        }
    }

private:
    // memory aligned to system pointer size
    uintptr_t data[(Size + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)];
};
