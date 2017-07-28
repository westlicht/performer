#pragma once

#include "sim/Simulator.h"
#include "sim/widgets/Button.h"
#include "sim/widgets/Led.h"

#include <vector>
#include <memory>
#include <deque>

#include <cstdint>

class ButtonLedMatrix {
public:
    enum Action {
        KeyDown,
        KeyUp,
    };
    struct Event {
        Event() = default;
        Event(Action action, int key) : _action(action), _key(key) {}

        Action action() const { return Action(_action); }
        int key() const { return _key; }

    private:
        uint8_t _action;
        uint8_t _key;
    };

    static const int Rows = 8;
    static const int ButtonCols = 5;
    static const int LedCols = 4;

    ButtonLedMatrix(sim::Simulator &simulator) :
        _simulator(simulator)
    {
        const sim::Vector2i matrixOrigin(40, 350);
        const sim::Vector2i buttonSize(20, 20);
        const sim::Vector2i ledSize(10, 10);
        const sim::Vector2i ledOffset(0, -20);
        const sim::Vector2i spacing(50, -40);

        const std::vector<SDL_Keycode> keys({
            SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b, SDLK_n, SDLK_m, SDLK_COMMA,
            SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g, SDLK_h, SDLK_j, SDLK_k,
            SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t, SDLK_y, SDLK_u, SDLK_i,
            SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8,
            SDLK_9, SDLK_0, SDLK_o, SDLK_p, SDLK_l, SDLK_l, SDLK_l, SDLK_l
        });

        for (int col = 0; col < ButtonCols; ++col) {
            for (int row = 0; row < Rows; ++row) {
                sim::Vector2i origin(matrixOrigin.x() + row * spacing.x(), matrixOrigin.y() + col * spacing.y());
                auto button = std::make_shared<sim::Button>(origin - buttonSize / 2, buttonSize, keys[col * Rows + row]);
                int index = col * Rows + row;
                button->setCallback([this, index] (bool pressed) {
                    _events.emplace_back(pressed ? KeyDown : KeyUp, index);
                });
                _buttons.emplace_back(button);
                _simulator.window().addWidget(button);
                if (col < LedCols) {
                    auto led = std::make_shared<sim::Led>(origin + ledOffset - ledSize / 2, ledSize);
                    _leds.emplace_back(led);
                    _simulator.window().addWidget(led);
                }
            }
        }
    }

    void setLed(int index, uint8_t red, uint8_t green) {
        if (index < _leds.size()) {
            _leds[index]->color() = sim::Color(red / 255.f, green / 255.f, 0.f, 1.f);
        }
    }

    inline void setLed(int row, int col, uint8_t red, uint8_t green) {
        setLed(col * Rows + row, red, green);
    }

    inline bool buttonState(int index) const {
        if (index < _buttons.size()) {
            return _buttons[index]->pressed();
        }
        return false;
    }

    inline bool buttonState(int row, int col) const {
        return buttonState(col * Rows + row);
    }

    inline bool nextEvent(Event &event) { 
        if (_events.empty()) {
            return false;
        }
        event = _events.front();
        _events.pop_front();
        return true;
    }

private:
    sim::Simulator &_simulator;
    std::vector<std::shared_ptr<sim::Button>> _buttons;
    std::vector<std::shared_ptr<sim::Led>> _leds;
    std::deque<Event> _events;
};
