#pragma once

#include <array>
#include <utility>
#include <functional>

template<typename T, size_t Capacity, typename Compare = std::less<T>>
class SortedQueue {
public:
    SortedQueue() {
        clear();
    }

    void clear() {
        _read = 0;
        _write = 0;
    }

    size_t capacity() {
        return Capacity;
    }

    size_t size() {
        return (_write - _read) % Capacity;
    }

    bool empty() const {
        return _write == _read;
    }

    void push(const T &value) {
        Compare compare;
        size_t pos = _write;
        _queue[pos] = value;

        // insert sort
        size_t cur = _write;
        size_t prev = decrease(cur);
        while (cur != _read && compare(_queue[cur], _queue[prev])) {
            std::swap(_queue[cur], _queue[prev]);
            cur = prev;
            prev = decrease(cur);
        }

        _write = increase(_write);
    }

    const T &front() const {
        return _queue[_read];
    }

    void pop() {
        _read = increase(_read);
    }

private:
    inline size_t increase(size_t pos) const {
        return (pos + 1) % Capacity;
    }

    inline size_t decrease(size_t pos) const {
        return (pos - 1) % Capacity;
    }

    T _queue[Capacity];
    size_t _read;
    size_t _write;
};
