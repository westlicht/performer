#pragma once

#include <bitset>

template<size_t N>
class RhythmString {
public:
    RhythmString() = default;
    RhythmString(size_t size) : _size(size) {}

    size_t capacity() const { return N; }

    size_t size() const { return _size; }

    void clear() {
        _size = 0;
        _steps.reset();
    }

    void resize(size_t size) {
        _size = size;
    }

    void set(size_t index, bool value = true) {
        _size = std::max(_size, index + 1);
        _steps.set(index, value);
    }

    RhythmString<N> shifted(size_t offset) const {
        RhythmString<N> shifted(_size);
        for (size_t i = 0; i < _size; ++i) {
            if (_steps[i]) {
                shifted.set((i + offset) % _size);
            }
        }
        return shifted;
    }

    void append(const RhythmString<N> &other) {
        for (size_t i = 0; i < other.size(); ++i) {
            _steps[_size + i] = other[i];
        }
        _size += other.size();
    }

    bool operator[](size_t index) const {
        return _steps[index];
    }

private:
    size_t _size = 0;
    std::bitset<N> _steps;
};
