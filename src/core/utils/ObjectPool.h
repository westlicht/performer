#pragma once

#include <array>

#include <cstdlib>

template<typename T, size_t Capacity>
class ObjectPool {
public:
    size_t capacity() const { return Capacity; }
    size_t size() { return _size; }

    T *allocate() {
        for (auto &item : _items) {
            if (!item.used) {
                item.used = true;
                --_size;
                return &item.object;
            }
        }
        return nullptr;
    }

    void release(T *object) {
        if (object) {
            Item *item = reinterpret_cast<Item *>(object);
            item->used = false;
            ++_size;
        }
    }

    template<typename Func>
    void forEach(Func func) {
        for (auto &item : _items) {
            if (item.used) {
                func(item.object);
            }
        }
    }

private:
    struct Item {
        T object;
        bool used = 0;
    };

    std::array<Item, Capacity> _items;
    size_t _size = Capacity;
};
