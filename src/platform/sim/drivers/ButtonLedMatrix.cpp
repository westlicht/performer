#include "ButtonLedMatrix.h"

ButtonLedMatrix::ButtonLedMatrix() :
    _simulator(sim::Simulator::instance())
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
        SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5
    });

    // button & leds
    for (int col = 0; col < ColsButton; ++col) {
        for (int row = 0; row < Rows; ++row) {
            if (col == ColsButton - 1 && row >= 5) {
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

            if (col < ColsLed) {
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

void ButtonLedMatrix::setLed(int index, uint8_t red, uint8_t green) {
    if (index < _leds.size()) {
        _leds[index]->color() = sim::Color(red / 255.f, green / 255.f, 0.f, 1.f);
    }
}

bool ButtonLedMatrix::buttonState(int index) const {
    if (index < _buttons.size()) {
        return _buttons[index]->pressed();
    }
    return false;
}

bool ButtonLedMatrix::nextEvent(Event &event) {
    if (_events.empty()) {
        return false;
    }
    event = _events.front();
    _events.pop_front();
    return true;
}
