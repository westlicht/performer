#include "Frontend.h"
#include "MidiConfig.h"

#include "widgets/Button.h"
#include "widgets/Led.h"

#include "instruments/DrumSampler.h"
#include "instruments/Synth.h"

#include "sim/TargetConfig.h"

#include <memory>
#include <sstream>
#include <iomanip>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace sim {

static Frontend *g_instance;

struct ColSettings {
    ColSettings(
        const Vector2i &origin = Vector2i(0, 0),
        const Vector2i &spacing = Vector2i(64, 0),
        int stride = 8,
        const Vector2i &buttonSize = Vector2i(20, 20),
        const Vector2i &ledSize = Vector2i(10, 10),
        const Vector2i &ledOffset = Vector2i(0, -20),
        const Vector2i &labelSize = Vector2i(50, 10),
        const Vector2i &labelOffset = Vector2i(0, 22)
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
    Vector2i origin;
    Vector2i spacing;
    int stride;
    Vector2i buttonSize;
    Vector2i ledSize;
    Vector2i ledOffset;
    Vector2i labelSize;
    Vector2i labelOffset;
};

static ColSettings colSettings[5] = {
//    origin                                spacing             stride  buttonSize
    { Vector2i(64 + 16 + 32, 380)                                                           },
    { Vector2i(64 + 16 + 32, 320)                                                           },
    { Vector2i(64 + 16 + 32, 260)                                                           },
    { Vector2i(64 + 16 + 512 + 64, 200),    Vector2i(64, 60),   2                           },
    { Vector2i(64 + 16 + 51, 200),          Vector2i(102, 0),   8,      Vector2i(40, 20)    },
};


Frontend::Frontend(Simulator &simulator) :
    _sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER),
    _simulator(simulator),
    _window("Sequencer", Vector2i(800, 500))
{
    g_instance = this;

    _timerFrequency = SDL_GetPerformanceFrequency();
    _timerStart = SDL_GetPerformanceCounter();

    setupWindow();
    setupMidi();
    setupInstruments();

    _simulator.registerTargetOutputObserver(this);
}

#ifdef __EMSCRIPTEN__
static void mainLoop() {
    g_instance->step();
}
#endif

void Frontend::run() {
    // _target.create();

#ifdef __EMSCRIPTEN__
    // 0 fps means to use requestAnimationFrame; non-0 means to use setTimeout.
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (!terminate()) {
        step();
    }
#endif

    // _target.destroy();
}

void Frontend::close() {
    _window.close();
}

bool Frontend::terminate() const {
    return _window.terminate();
}

void Frontend::step() {
    update();
    render();
#ifdef __EMSCRIPTEN__
    delay(1);
#endif
}

void Frontend::update() {
    uint32_t currentTicks = std::floor(ticks());
    for (uint32_t tick = _lastUpdateTicks; tick < currentTicks; ++tick) {
        _simulator.wait(1);
    }
    _lastUpdateTicks = currentTicks;

    _midi.update();
    _window.update();
}

void Frontend::render() {
    double currentTicks = ticks();
    if (currentTicks < _lastRenderTicks + 15) {
        return;
    }
    _lastRenderTicks = currentTicks;
    _window.render();
}

void Frontend::delay(int ms) {
    SDL_Delay(ms);
}

double Frontend::ticks() const {
    double delta = SDL_GetPerformanceCounter() - _timerStart;
    return delta / _timerFrequency * 1000.0;
}

void Frontend::setupWindow() {
    setupEncoder();
    setupLcd();
    setupButtonLedMatrix();
    setupAdc();
    setupDio();

    auto screenshotButton = _window.createWidget<Button>(
        Vector2i(8, 8),
        Vector2i(8, 8),
        SDLK_F10
    );

    screenshotButton->setCallback([&] (bool pressed) {
        // if (pressed) {
        //     screenshot();
        // }
    });
}

void Frontend::setupEncoder() {
    auto encoder = _window.createWidget<Encoder>(
        Vector2i(8, 8 + 32),
        Vector2i(64, 64),
        SDLK_SPACE
    );

    encoder->setButtonCallback([this] (bool pressed) {
        _simulator.writeEncoder(pressed ? EncoderEvent::Down : EncoderEvent::Up);
    });

    encoder->setValueCallback([this] (int value) {
        if (value > 0) {
            for (int i = 0; i < value; ++i) {
                _simulator.writeEncoder(EncoderEvent::Right);
            }
        } else if (value < 0) {
            for (int i = 0; i > value; --i) {
                _simulator.writeEncoder(EncoderEvent::Left);
            }
        }
    });
}

void Frontend::setupLcd() {
    Vector2i resolution(TargetConfig::LcdWidth, TargetConfig::LcdHeight);
    _display = _window.createWidget<Display>(
        Vector2i(64 + 16, 8),
        resolution * 2 + Vector2i(2, 2),
        resolution
    );
}

void Frontend::setupButtonLedMatrix() {
    const std::vector<SDL_Keycode> keys({
        SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b, SDLK_n, SDLK_m, SDLK_COMMA,
        SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g, SDLK_h, SDLK_j, SDLK_k,
        SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t, SDLK_y, SDLK_u, SDLK_i,
        SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_LEFT, SDLK_RIGHT, SDLK_LSHIFT, SDLK_LALT,
        SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5
    });

    const std::vector<std::string> labels({
        "S9", "S10", "S11", "S12", "S13", "S14", "S15", "S16",
        "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8",
        "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8",
        "PLAY", "TEMPO", "PATT", "PERF", "<", ">", "SHIFT", "PAGE",
        "", "", "", "", ""
    });

    for (int col = 0; col < TargetConfig::ColsButton; ++col) {
        const auto &settings = colSettings[col];
        for (int row = 0; row < TargetConfig::Rows; ++row) {
            if (col == TargetConfig::ColsButton - 1 && row >= 5) {
                continue;
            }

            int index = col * TargetConfig::Rows + row;

            Vector2i origin(
                settings.origin.x() + (row % settings.stride) * settings.spacing.x(),
                settings.origin.y() + (row / settings.stride) * settings.spacing.y()
            );

            // button
            auto button = _window.createWidget<Button>(
                origin - settings.buttonSize / 2,
                settings.buttonSize,
                keys[index]
            );
            button->setCallback([this, index] (bool pressed) {
                _simulator.writeButton(index, pressed);
            });
            _buttons.emplace_back(button);

            // button label
            auto buttonLabel = _window.createWidget<Label>(
                origin - settings.buttonSize / 2,
                settings.buttonSize,
                SDL_GetKeyName(keys[index]),
                Font("inconsolata", 12),
                Color(0.5f, 1.f)
            );
            _labels.emplace_back(buttonLabel);

            // led
            if (col < TargetConfig::ColsLed) {
                auto led = _window.createWidget<Led>(
                    origin + settings.ledOffset - settings.ledSize / 2,
                    settings.ledSize
                );
                _leds.emplace_back(led);
            }

            auto label = _window.createWidget<Label>(
                origin + settings.labelOffset - settings.labelSize / 2,
                settings.labelSize,
                labels[index],
                Font("inconsolata", 16),
                Color(1.f, 1.f)
            );
            _labels.emplace_back(label);
        }
    }
}

void Frontend::setupAdc() {
    for (int i = 0; i < TargetConfig::AdcChannels; ++i) {
        auto rotary = _window.createWidget<Rotary>(Vector2i(50 + i * 50, 450), Vector2i(40, 40));
        rotary->setValueCallback([this, i] (float value) {
            _simulator.setAdc(i, value);
        });
        _simulator.setAdc(i, 0.5f);
    }
}

void Frontend::setupDio() {
    // clock input
    auto button = _window.createWidget<Button>(
        Vector2i(600, 100),
        Vector2i(20, 20),
        SDLK_F10
    );

    _clockSource.reset(new ClockSource(_simulator, [this] () {
        _simulator.writeDigitalInput(0, true);
        _simulator.writeDigitalInput(0, false);
    }));

    button->setCallback([this] (bool pressed) {
        if (pressed) {
            _clockSource->toggle();
        }
    });

    // reset input
    button = _window.createWidget<Button>(
        Vector2i(600, 130),
        Vector2i(20, 20),
        SDLK_F11
    );
    button->setCallback([this] (bool pressed) {
        _simulator.writeDigitalInput(1, pressed);
    });

    // clock output
    _clockOutputLed = _window.createWidget<Led>(
        Vector2i(630, 100),
        Vector2i(20, 20),
        Color(0.f, 1.f)
    );

    // reset output
    _resetOutputLed = _window.createWidget<Led>(
        Vector2i(630, 130),
        Vector2i(20, 20),
        Color(0.f, 1.f)
    );
}

void Frontend::setupMidi() {
    _midiPort = std::make_shared<Midi::Port>(
        midiPortConfig.port,
        [this] (const std::vector<uint8_t> &message) {
            if (message.size() >= 1 && message.size() <= 3) {
                _simulator.writeMidiInput(MidiEvent::makeMessage(0, MidiMessage(message.data(), message.size())));
            }
        }
    );

    _midi.registerPort(_midiPort);

    _usbMidiPort = std::make_shared<sim::Midi::Port>(
        usbMidiPortConfig.port,
        [this] (const std::vector<uint8_t> &message) {
            if (message.size() >= 1 && message.size() <= 3) {
                _simulator.writeMidiInput(MidiEvent::makeMessage(1, MidiMessage(message.data(), message.size())));
            }
        },
        [this] () {
            _simulator.writeMidiInput(MidiEvent::makeConnect(1, usbMidiPortConfig.vendorId, usbMidiPortConfig.productId));
        },
        [this] () {
            _simulator.writeMidiInput(MidiEvent::makeDisconnect(1));
        }
    );

    _midi.registerPort(_usbMidiPort);
}

void Frontend::setupInstruments() {
    // _instruments.reset(new SamplerSetup(_audio));
    _instruments.reset(new MixedSetup(_audio));
}

void Frontend::writeLed(int index, bool red, bool green) {
    if (index < int(_leds.size())) {
        _leds[index]->color() = Color(red ? 1.f : 0.f, green ? 1.f : 0.f, 0.f, 1.f);
    }
}

void Frontend::writeGateOutput(int channel, bool value) {
    _instruments->setGate(channel, value);
}

void Frontend::writeDac(int channel, uint16_t value) {
    auto valueToVolts = [] (uint16_t value) {
        // In ideal DAC/OpAmp configuration we get:
        // 0     ->  5.17V
        // 32768 -> -5.25V
        // it follows:
        // 534   ->  5.00V
        // 31981 -> -5.00V
        static constexpr float value0 = 31981.f;
        static constexpr float value1 = 534.f;
        return (value - value0) / (value1 - value0) * 10.f - 5.f;
    };

    _instruments->setCv(channel, valueToVolts(value));
}

void Frontend::writeDigitalOutput(int pin, bool value) {
    switch (pin) {
    case 0:
        _clockOutputLed->color() = Color(value ? 1.f : 0.f, 1.f);
        break;
    case 1:
        _resetOutputLed->color() = Color(value ? 1.f : 0.f, 1.f);
        break;
    }
}

void Frontend::writeLcd(uint8_t *frameBuffer) {
    _display->draw(frameBuffer);
}

void Frontend::writeMidiOutput(MidiEvent event) {
    if (event.kind == MidiEvent::Message) {
        const auto &message = event.message;
        switch (event.port) {
        case 0:
            _midiPort->send(message.raw(), message.length());
            break;
        case 1:
            _usbMidiPort->send(message.raw(), message.length());
            break;
        }
    }
}

} // namespace sim
