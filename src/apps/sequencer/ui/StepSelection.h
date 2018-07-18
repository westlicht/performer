#pragma once

#include "Key.h"

#include <bitset>

#include <cstdint>
#include <cstdlib>

template<size_t N>
class StepSelection {
public:
    void keyDown(KeyEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (key.pageModifier() || key.shiftModifier()) {
            return;
        }

        if (key.isStep()) {
            // switch to immediate mode
            if (_mode == Mode::Persist) {
                _selected.reset();
                _mode = Mode::Immediate;
            }

            int stepIndex = stepOffset + key.step();

            if (_selected.none()) {
                _first = stepIndex;
            }

            _selected.set(stepIndex);

            event.consume();
        }
    }

    void keyUp(KeyEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (key.pageModifier() || key.shiftModifier()) {
            return;
        }

        if (key.isStep()) {
            int stepIndex = stepOffset + key.step();

            _selected.reset(stepIndex);

            if (_first >= 0 && !_selected[_first]) {
                _first = firstSetIndex();
            }

            event.consume();
        }
    }

    void keyPress(KeyPressEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (key.pageModifier() || !key.shiftModifier()) {
            return;
        }

        if (_mode == Mode::Persist && key.isShift() && event.count() == 2) {
            clear();
            event.consume();
            return;
        }

        if (key.isStep()) {
            int stepIndex = stepOffset + key.step();

            // switch to persist mode
            if (_mode == Mode::Immediate) {
                _selected.reset();
                _mode = Mode::Persist;
                _first = -1;
            }

            int count = event.count();

            if (count == 1) {
                if (_selected[stepIndex]) {
                    _selected[stepIndex] = false;
                    if (_first == stepIndex) {
                        _first = firstSetIndex();
                    }
                } else {
                    _selected[stepIndex] = true;
                    if (_first == -1) {
                        _first = stepIndex;
                    }
                }
            } else if (count == 2) {
                int otherStep = otherPressedStepKey(key.state(), key.step());
                if (otherStep >= 0) {
                    int firstIndex = stepOffset + otherStep;
                    int interval = stepIndex - firstIndex;
                    for (int i = firstIndex; i >= 0 && i < int(_selected.size()); i += interval) {
                        _selected[i] = true;
                    }
                } else {
                    // TODO select all "equal" steps
                }
            }

            event.consume();
        }
    }

    void clear() {
        _selected.reset();
        _mode = Mode::Immediate;
    }

    bool isPersisted() const {
        return _mode == Mode::Persist;
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

    size_t size() const {
        return _selected.size();
    }

    size_t count() const {
        return _selected.count();
    }

    const std::bitset<N> &selected() const { return _selected; }

    bool operator[](int index) const {
        return _selected[index];
    }

private:
    int otherPressedStepKey(const KeyState &keyState, int step) const {
        bool found = false;
        int other = -1;
        for (int i = 0; i < 16; ++i) {
            if (i != step && keyState[MatrixMap::fromStep(i)]) {
                if (found) {
                    return -1;
                } else {
                    other = i;
                    found = true;
                }
            }
        }
        return other;
    }

    int firstSetIndex() const {
        for (size_t i = 0; i < _selected.size(); ++i) {
            if (_selected[i]) {
                return i;
            }
        }
        return -1;
    }

    enum class Mode : uint8_t {
        Immediate,
        Persist,
    };

    Mode _mode = Mode::Immediate;
    std::bitset<N> _selected;
    int _first = -1;
    int8_t _lastPressedIndex;
};
