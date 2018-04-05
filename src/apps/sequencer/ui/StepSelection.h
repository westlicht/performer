#pragma once

#include "Key.h"

#include <bitset>

#include <cstdint>
#include <cstdlib>

template<size_t N>
class StepSelection {
public:
    enum class Mode : uint8_t {
        Immediate,
        Persist,
    };


    void keyDown(KeyEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (key.shiftModifier()) {
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

        if (key.shiftModifier()) {
            return;
        }

        if (key.isStep()) {
            int stepIndex = stepOffset + key.step();

            _selected.reset(stepIndex);

            if (!_selected[_first]) {
                _first = firstSetIndex();
            }

            event.consume();
        }
    }

    void keyPress(KeyPressEvent &event, int stepOffset) {
        const auto &key = event.key();

        if (!key.shiftModifier()) {
            return;
        }

        if (key.isStep()) {
            // switch to persist mode
            if (_mode == Mode::Immediate) {
                _selected.reset();
                _mode = Mode::Persist;
            }

            int stepIndex = stepOffset + key.step();
            int count = event.count();

            if (count == 1) {
                if (!otherStepKeyPressed(key.state(), key.step())) {
                    _selected.reset();
                    _first = stepIndex;
                }
                _selected[stepIndex] = true;
            } else if (count == 2) {
                if (_selected.count() == 2) {
                    int interval = stepIndex - _first;
                    for (int i = stepIndex; i >= 0 && i < int(_selected.size()); i += interval) {
                        _selected[i] = true;
                    }
                } else if (stepIndex == 0) {
                    _selected.set();
                }
            }

            event.consume();
        }
    }

    void clear() {
        _selected.reset();
        _mode = Mode::Immediate;
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

    const std::bitset<N> &selected() const { return _selected; }

    bool operator[](int index) const {
        return _selected[index];
    }

private:
    bool otherStepKeyPressed(const KeyState &keyState, int step) const {
        for (int i = 0; i < 16; ++i) {
            if (i != step && keyState[MatrixMap::fromStep(i)]) {
                return true;
            }
        }
        return false;
    }

    int firstSetIndex() const {
        for (size_t i = 0; i < _selected.size(); ++i) {
            if (_selected[i]) {
                return i;
            }
        }
        return 0;
    }

    Mode _mode = Mode::Immediate;
    std::bitset<N> _selected;
    int _first = 0;
};
