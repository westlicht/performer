#pragma once

#include "sim/Simulator.h"
#include "sim/widgets/Button.h"
#include "sim/widgets/Led.h"
#include "sim/widgets/Encoder.h"

#include <vector>
#include <deque>

#include <cstdint>

class ButtonLedMatrix {
public:
    enum Action {
        KeyDown,
        KeyUp,
        Encoder,
    };
    struct Event {
        Event() = default;
        Event(Action action, int value) : _action(action), _value(value) {}

        Action action() const { return Action(_action); }
        int value() const { return _value; }

    private:
        uint8_t _action;
        int8_t _value;
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
            SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_LSHIFT,
            SDLK_9, SDLK_0, SDLK_o, SDLK_p, SDLK_l
        });

        // button & leds
        for (int col = 0; col < ButtonCols; ++col) {
            for (int row = 0; row < Rows; ++row) {
                if (col == ButtonCols - 1 && row >= 5) {
                    continue;
                }
                sim::Vector2i origin(matrixOrigin.x() + row * spacing.x(), matrixOrigin.y() + col * spacing.y());

                auto button = _simulator.window().createWidget<sim::Button>(
                    origin - buttonSize / 2,
                    buttonSize,
                    keys[col * Rows + row]
                );
                int index = col * Rows + row;
                button->setCallback([this, index] (bool pressed) {
                    _events.emplace_back(pressed ? KeyDown : KeyUp, index);
                });
                _buttons.emplace_back(button);

                if (col < LedCols) {
                    auto led = _simulator.window().createWidget<sim::Led>(
                        origin + ledOffset - ledSize / 2,
                        ledSize
                    );
                    _leds.emplace_back(led);
                }
            }
        }

        // encoder
        _encoder = _simulator.window().createWidget<sim::Encoder>(
            sim::Vector2i(10, 10),
            sim::Vector2i(60, 60)
        );
        _encoder->setButtonCallback([this] (bool pressed) {
            std::cout << "pressed: " << pressed << std::endl;
        });
        _encoder->setValueCallback([this] (int value) {
            _events.emplace_back(Encoder, value);
        });
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
    std::vector<sim::Button::Ptr> _buttons;
    std::vector<sim::Led::Ptr> _leds;
    sim::Encoder::Ptr _encoder;
    std::deque<Event> _events;
};
