#pragma once

#include <bitset>

#include <cstdlib>

template<size_t N>
class StepSelection {
public:
    StepSelection() {
    }

    void add(int step) {
        if (_selected.none()) {
            _first = step;
        }
        _selected[step] = true;
    }

    void remove(int step) {
        _selected[step] = false;
        if (_selected.none()) {
            _first = 0;
        } else if (!_selected[_first]) {
            for (size_t i = 0; i < _selected.size(); ++i) {
                if (_selected[i]) {
                    _first = i;
                    break;
                }
            }
        }
    }

    int first() const {
        return _first;
    }

    bool none() const {
        return _selected.none();
    }

    bool any() const {
        return _selected.any();
    }

    bool operator[](int index) const {
        return _selected[index];
    }

private:
    std::bitset<N> _selected;
    int _first = 0;
};
