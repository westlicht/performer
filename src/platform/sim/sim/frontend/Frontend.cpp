#include "Frontend.h"
#include "MidiConfig.h"
#include "Frontpanel.h"

#include "widgets/Button.h"
#include "widgets/Led.h"
#include "widgets/Image.h"
#include "widgets/Panel.h"

#include "instruments/DrumSampler.h"
#include "instruments/Synth.h"

#include "sim/TargetConfig.h"
#include "sim/TargetUtils.h"

#include "args.hxx"
#include "tinyformat.h"

#include <memory>
#include <sstream>
#include <iomanip>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace sim {

#ifdef __EMSCRIPTEN__
static Frontend *g_instance;
#endif

template<typename T>
static void addWidget(std::vector<T> &list, T widget, int index) {
    list.resize(std::max(int(list.size()), index + 1));
    list[index] = widget;
}

Frontend::Frontend(Simulator &simulator) :
    _simulator(simulator)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

#ifdef __EMSCRIPTEN__
    g_instance = this;
#endif
}

Frontend::~Frontend() {
    SDL_Quit();
}

int Frontend::main(int argc, char *argv[]) {
    args::ArgumentParser parser("PER|FORMER Simulator", "");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::Flag showMidiPorts(parser, "midi", "Show available MIDI ports", { 'm', "midi" });

    try {
        parser.ParseCLI(argc, argv);
    } catch (const args::Help &) {
        std::cout << parser;
        return 0;
    } catch (const args::ParseError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (showMidiPorts) {
        _midi.dumpPorts();
        return 0;
    }



    run();

    return 0;
}

#ifdef __EMSCRIPTEN__
void emscriptenMainLoop() {
    g_instance->step();
}
#endif

void Frontend::run() {
    setup();

#ifdef __EMSCRIPTEN__
    // 0 fps means to use requestAnimationFrame; non-0 means to use setTimeout.
    emscripten_set_main_loop(emscriptenMainLoop, 0, 1);
#else
    while (!terminate()) {
        step();
    }
#endif
}

void Frontend::close() {
    _window->close();
}

bool Frontend::terminate() const {
    return _window->terminate();
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
    _window->update();
}

void Frontend::render() {
    double currentTicks = ticks();
    if (currentTicks < _lastRenderTicks + 15) {
        return;
    }
    _lastRenderTicks = currentTicks;
    _window->render();
}

void Frontend::delay(int ms) {
    SDL_Delay(ms);
}

double Frontend::ticks() const {
    double delta = SDL_GetPerformanceCounter() - _timerStart;
    return delta / _timerFrequency * 1000.0;
}

void Frontend::setup() {
    _timerFrequency = SDL_GetPerformanceFrequency();
    _timerStart = SDL_GetPerformanceCounter();

#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    setupWindow();
    setupMidi();
    setupInstruments();

    _simulator.registerTargetInputObserver(this);
    _simulator.registerTargetOutputObserver(this);
}

void Frontend::setupWindow() {
    Vector2i size(Frontpanel::windowWidth, Frontpanel::windowHeight + Frontpanel::controlHeight);
    _window = std::make_shared<Window>("PER|FORMER Simulator", size);

    setupFrontpanel();
    setupControls();
}

void Frontend::setupFrontpanel() {
    const std::vector<SDL_Keycode> keys({
        SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b, SDLK_n, SDLK_m, SDLK_COMMA,
        SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g, SDLK_h, SDLK_j, SDLK_k,
        SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t, SDLK_y, SDLK_u, SDLK_i,
        SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_LEFT, SDLK_RIGHT, SDLK_LSHIFT, SDLK_LALT,
        SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5
    });

    const double scale = Frontpanel::scale;

    auto transformToScreen = [&] (double x, double y) {
        return Vector2f(x * scale, y * scale);
    };

    auto scaleToScreen = [&] (double w, double h) {
        return Vector2f(w * scale, h * scale);
    };

    _window->createWidget<Image>(Vector2f(0.f, 0.f), scaleToScreen(Frontpanel::width, Frontpanel::height), "assets/frontpanel.png");

    for (const auto &info : Frontpanel::infos) {
        auto origin = transformToScreen(info.x, info.y);
        auto size = scaleToScreen(info.w, info.h);

        switch (info.widget) {
        case Frontpanel::Widget::Jack: {
            auto jack = _window->createWidget<Jack>(origin - size / 2, size);
            switch (info.signal) {
            case Frontpanel::Signal::MidiInput:
                _midiInputJack = jack;
                break;
            case Frontpanel::Signal::MidiOutput:
                _midiOutputJack = jack;
                break;
            case Frontpanel::Signal::DigitalInput:
                addWidget(_digitalInputJacks, jack, info.index);
                break;
            case Frontpanel::Signal::DigitalOutput:
                addWidget(_digitalOutputJacks, jack, info.index);
                break;
            case Frontpanel::Signal::CvInput:
                addWidget(_cvInputJacks, jack, info.index);
                break;
            case Frontpanel::Signal::GateOutput:
                addWidget(_gateOutputJacks, jack, info.index);
                break;
            case Frontpanel::Signal::CvOutput:
                addWidget(_cvOutputJacks, jack, info.index);
                break;
            default:
                break;
            }
            break;
        }
        case Frontpanel::Widget::Led: {
            auto led = _window->createWidget<Led>(origin - size / 2, size);
            if (info.signal == Frontpanel::Signal::Led) {
                addWidget(_leds, led, info.index);
            }
            break;
        }
        case Frontpanel::Widget::CButton:
        case Frontpanel::Widget::RButton: {
            auto button = _window->createWidget<Button>(
                origin - size / 2,
                size,
                info.widget == Frontpanel::Widget::CButton ? Button::Ellipse : Button::Rectangle,
                keys[info.index]
            );
            if (info.signal == Frontpanel::Signal::Button) {
                button->setCallback([this, info] (bool pressed) {
                    _simulator.writeButton(info.index, pressed);
                });
                addWidget(_buttons, button, info.index);
            }
            break;
        }
        case Frontpanel::Widget::Encoder: {
            _encoder = _window->createWidget<Encoder>(origin - size / 2, size, SDLK_SPACE);
            _encoder->setButtonCallback([this] (bool pressed) {
                _simulator.writeEncoder(pressed ? EncoderEvent::Down : EncoderEvent::Up);
            });

            _encoder->setValueCallback([this] (int value) {
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
            break;
        }
        case Frontpanel::Widget::Lcd: {
            Vector2i resolution(TargetConfig::LcdWidth, TargetConfig::LcdHeight);
            _lcd = _window->createWidget<Display>(origin, size, resolution);
            break;
        }
        }
    }
}

void Frontend::setupControls() {
    int x = 10;
    int y = Frontpanel::windowHeight;

    // panel
    _window->createWidget<Panel>(Vector2f(0.f, Frontpanel::windowHeight), Vector2f(Frontpanel::windowWidth, Frontpanel::controlHeight), Color(1.f, 1.f, 1.f, 0.05f));

    // cv inputs
    for (int i = 0; i < TargetConfig::AdcChannels; ++i) {
        auto rotary = _window->createWidget<Rotary>(Vector2f(x, y + 10), Vector2f(40, 40));
        rotary->setValueCallback([this, i] (float value) {
            _simulator.setAdc(i, value * 10.f - 5.f);
        });
        _simulator.setAdc(i, 0.f);
        _window->createWidget<Label>(Vector2f(x, y + 60), Vector2f(40, 10), tfm::format("CV%d IN", i + 1));
        x += 50;
    }

    // clock input
    {
        auto button = _window->createWidget<Button>(
            Vector2f(x + 10, y + 20),
            Vector2f(20, 20),
            Button::Rectangle,
            SDLK_F10
        );
        _window->createWidget<Label>(Vector2f(x, y + 60), Vector2f(40, 10), "CLK IN");
        x += 50;

        _clockSource.reset(new ClockSource(_simulator, [this] () {
            _simulator.writeDigitalInput(0, true);
            _simulator.writeDigitalInput(0, false);
        }));

        button->setCallback([this] (bool pressed) {
            if (pressed) {
                _clockSource->toggle();
            }
        });
    }

    // reset input
    {
        auto button = _window->createWidget<Button>(
            Vector2f(x + 10, y + 20),
            Vector2f(20, 20),
            Button::Rectangle,
            SDLK_F11
        );
        _window->createWidget<Label>(Vector2f(x, y + 60), Vector2f(40, 10), "RST IN");
        x += 50;

        button->setCallback([this] (bool pressed) {
            _simulator.writeDigitalInput(1, pressed);
        });
    }

    // screenshot
    {
        auto button = _window->createWidget<Button>(
            Vector2f(x + 10, y + 20),
            Vector2f(40, 20),
            Button::Rectangle,
            SDLK_F12
        );
        _window->createWidget<Label>(Vector2f(x, y + 60), Vector2f(60, 10), "SCREENSHOT");
        x += 70;

        button->setCallback([&] (bool pressed) {
            if (pressed) {
                _simulator.screenshot("screenshot.png");
            }
        });

    }
}

// // button label
// auto buttonLabel = _window->createWidget<Label>(
//     origin - settings.buttonSize / 2,
//     settings.buttonSize,
//     SDL_GetKeyName(keys[index]),
//     Font("inconsolata", 12),
//     Color(0.5f, 1.f)
// );
// _labels.emplace_back(buttonLabel);

void Frontend::setupMidi() {
    _midiPort = std::make_shared<Midi::Port>(
        midiPortConfig.portIn,
        midiPortConfig.portOut,
        [this] (const std::vector<uint8_t> &message) {
            if (message.size() >= 1 && message.size() <= 3) {
                _simulator.writeMidiInput(MidiEvent::makeMessage(0, MidiMessage(message.data(), message.size())));
            }
        }
    );

    _midi.registerPort(_midiPort);

    _usbMidiPort = std::make_shared<sim::Midi::Port>(
        usbMidiPortConfig.portIn,
        usbMidiPortConfig.portOut,
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

// TargetInputHandler

void Frontend::writeButton(int index, bool pressed) {
    if (index >= 0 && index < int(_buttons.size())) {
        _buttons[index]->setState(pressed);
    }
}

void Frontend::writeEncoder(EncoderEvent event) {
}

void Frontend::writeAdc(int channel, uint16_t value) {
    if (channel >= 0 && channel < int(_cvInputJacks.size())) {
        float voltage = adcToVoltage(value);
        _cvInputJacks[channel]->setValue(voltage, -5.f, 5.f);
    }
}

void Frontend::writeDigitalInput(int pin, bool value) {
    if (pin >= 0 && pin < int(_digitalInputJacks.size())) {
        _digitalInputJacks[pin]->setState(value);
    }
}

void Frontend::writeMidiInput(MidiEvent event) {
}

// TargetOutputHandler

void Frontend::writeLed(int index, bool red, bool green) {
    if (index >= 0 && index < int(_leds.size())) {
        _leds[index]->color() = Color(red ? 1.f : 0.f, green ? 1.f : 0.f, 0.f, 1.f);
    }
}

void Frontend::writeGateOutput(int channel, bool value) {
    _instruments->setGate(channel, value);
    if (channel >= 0 && channel < int(_gateOutputJacks.size())) {
        _gateOutputJacks[channel]->setState(value);
    }
}

void Frontend::writeDac(int channel, uint16_t value) {
    float voltage = dacToVoltage(value);
    _instruments->setCv(channel, voltage);
    if (channel >= 0 && channel < int(_cvOutputJacks.size())) {
        _cvOutputJacks[channel]->setValue(voltage, -5.f, 5.f);
    }
}

void Frontend::writeDigitalOutput(int pin, bool value) {
    if (pin >= 0 && pin < int(_digitalOutputJacks.size())) {
        _digitalOutputJacks[pin]->setState(value);
    }
}

void Frontend::writeLcd(const FrameBuffer &frameBuffer) {
    _lcd->draw(frameBuffer.data());
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
