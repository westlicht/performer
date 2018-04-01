#include "ButtonLedMatrix.h"

struct ColSettings {
    ColSettings(
        const sim::Vector2i &origin = sim::Vector2i(0, 0),
        const sim::Vector2i &spacing = sim::Vector2i(64, 0),
        int stride = 8,
        const sim::Vector2i &buttonSize = sim::Vector2i(20, 20),
        const sim::Vector2i &ledSize = sim::Vector2i(10, 10),
        const sim::Vector2i &ledOffset = sim::Vector2i(0, -20),
        const sim::Vector2i &labelSize = sim::Vector2i(50, 10),
        const sim::Vector2i &labelOffset = sim::Vector2i(0, 22)
    ) {
        this->origin = origin;
        this->spacing = spacing;
        this->stride = stride;
        this->buttonSize = buttonSize;
        this->ledSize = ledSize;
        this->ledOffset = ledOffset;
        this->labelSize = labelSize;
        this->labelOffset = labelOffset;
    }
    sim::Vector2i origin;
    sim::Vector2i spacing;
    int stride;
    sim::Vector2i buttonSize;
    sim::Vector2i ledSize;
    sim::Vector2i ledOffset;
    sim::Vector2i labelSize;
    sim::Vector2i labelOffset;
};

static ColSettings colSettings[5] = {
//    origin                                    spacing                 stride  buttonSize
    { sim::Vector2i(64 + 16 + 32, 380)                                                                  },
    { sim::Vector2i(64 + 16 + 32, 320)                                                                  },
    { sim::Vector2i(64 + 16 + 32, 260)                                                                  },
    { sim::Vector2i(64 + 16 + 512 + 64, 200),   sim::Vector2i(64, 60),  2                               },
    { sim::Vector2i(64 + 16 + 51, 200),         sim::Vector2i(102, 0),  8,      sim::Vector2i(40, 20)   },
};

ButtonLedMatrix::ButtonLedMatrix() :
    _simulator(sim::Simulator::instance())
{
    const std::vector<SDL_Keycode> keys({
        SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b, SDLK_n, SDLK_m, SDLK_COMMA,
        SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g, SDLK_h, SDLK_j, SDLK_k,
        SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t, SDLK_y, SDLK_u, SDLK_i,
        SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_LEFT, SDLK_RIGHT, SDLK_LALT, SDLK_LSHIFT,
        SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5
    });

    const std::vector<std::string> labels({
        "S9", "S10", "S11", "S12", "S13", "S14", "S15", "S16",
        "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8",
        "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8",
        "PLAY", "TEMPO", "SNAP", "PERF", "<", ">", "PAGE", "SHIFT",
        "", "", "", "", ""
    });

    // button & leds
    for (int col = 0; col < ColsButton; ++col) {
        const auto &settings = colSettings[col];
        for (int row = 0; row < Rows; ++row) {
            if (col == ColsButton - 1 && row >= 5) {
                continue;
            }

            int index = col * Rows + row;

            sim::Vector2i origin(
                settings.origin.x() + (row % settings.stride) * settings.spacing.x(),
                settings.origin.y() + (row / settings.stride) * settings.spacing.y()
            );

            // button
            auto button = _simulator.window().createWidget<sim::Button>(
                origin - settings.buttonSize / 2,
                settings.buttonSize,
                keys[index]
            );
            button->setCallback([this, index] (bool pressed) {
                _events.emplace_back(pressed ? Event::KeyDown : Event::KeyUp, index);
            });
            _buttons.emplace_back(button);

            // button label
            auto buttonLabel = _simulator.window().createWidget<sim::Label>(
                origin - settings.buttonSize / 2,
                settings.buttonSize,
                SDL_GetKeyName(keys[index]),
                sim::Font("inconsolata", 12),
                sim::Color(0.5f, 1.f)
            );
            _labels.emplace_back(buttonLabel);

            // led
            if (col < ColsLed) {
                auto led = _simulator.window().createWidget<sim::Led>(
                    origin + settings.ledOffset - settings.ledSize / 2,
                    settings.ledSize
                );
                _leds.emplace_back(led);
            }

            auto label = _simulator.window().createWidget<sim::Label>(
                origin + settings.labelOffset - settings.labelSize / 2,
                settings.labelSize,
                labels[index],
                sim::Font("inconsolata", 16),
                sim::Color(1.f, 1.f)
            );
            _labels.emplace_back(label);
        }
    }
}

void ButtonLedMatrix::setLed(int index, uint8_t red, uint8_t green) {
    if (index < int(_leds.size())) {
        _leds[index]->color() = sim::Color(red / 255.f, green / 255.f, 0.f, 1.f);
    }
}

bool ButtonLedMatrix::buttonState(int index) const {
    if (index < int(_buttons.size())) {
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
