#include "drivers/Adc.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/Console.h"
#include "drivers/Dac.h"
#include "drivers/Dio.h"
#include "drivers/Encoder.h"
#include "drivers/GateOutput.h"
#include "drivers/Lcd.h"
#include "drivers/Midi.h"
#include "drivers/System.h"
#include "drivers/ShiftRegister.h"
#include "drivers/DebugLed.h"
#include "drivers/HighResolutionTimer.h"
#include "drivers/UsbH.h"
#include "drivers/UsbMidi.h"
#include "drivers/ClockTimer.h"
#include "drivers/SdCard.h"

#include "os/os.h"

#include "core/fs/Volume.h"
#include "core/gfx/FrameBuffer.h"
#include "core/gfx/Canvas.h"
#include "core/utils/RingBuffer.h"
#include "core/utils/StringBuilder.h"
#include "core/midi/MidiMessage.h"

#include "../hwconfig/HardwareConfig.h"

#include "LogBuffer.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

extern "C" {
__attribute__ ((section(".version")))
struct {
    uint32_t magic = 0xfadebabe;
    char name[24] = "Tester";
    uint8_t major = 0;
    uint8_t minor = 1;
    uint16_t revision = 0;
} version_tag;
} // extern "C"

static ClockTimer clockTimer;
static ShiftRegister shiftRegister;
static ButtonLedMatrix blm(shiftRegister, HardwareConfig::invertLeds());
static Encoder encoder(HardwareConfig::reverseEncoder());
static Lcd lcd;
static Adc adc;
static Dac dac;
static Dio dio;
static GateOutput gateOutput(shiftRegister);
static Midi midi;
static UsbMidi usbMidi;
static UsbH usbh(usbMidi);
static SdCard sdCard;

static fs::Volume volume(sdCard);

class TesterTask {
public:
    enum class Mode {
        Intro,
        Buttons,
        Leds,
        Midi,
        Clock,
        CvInputs,
        CvOutputs,
        GateOutputs,
        Last
    };

    enum class LEDColor : uint8_t {
        Off,
        Red,
        Green,
        Orange,
        Last
    };

    enum class MidiPort {
        Midi,
        UsbMidi
    };

    TesterTask() :
        _frameBuffer(256, 64, _frameBufferData),
        _canvas(_frameBuffer, _brightness)
    {
        _cvOutputs.fill(0);
        _gateOutputs.fill(false);
    }

    void init() {
        setMode(Mode(0));
    }

    void update() {
        processEncoder();
        processButtons();
        processMidi();

        uint32_t ticks = os::ticks();
        if (ticks - _lastRefresh > os::time::ms(100)) {
            _lastRefresh = ticks;
            refresh();
        }

        updateLeds();
        updateDio();
        updateCvOutputs();
        updateGateOutputs();
        draw();
    }

private:
    void processEncoder() {
        Encoder::Event event;
        while (encoder.nextEvent(event)) {
            encoderEvent(event);
        }
    }

    void processButtons() {
        ButtonLedMatrix::Event event;
        while (blm.nextEvent(event)) {
            buttonEvent(event);
        }
    }

    void processMidi() {
        uint8_t cable;
        MidiMessage message;

        while (midi.recv(&message)) {
            midiReceive(MidiPort::Midi, message);
        }

        while (usbMidi.recv(&cable, &message)) {
            if (cable == 0) {
                midiReceive(MidiPort::UsbMidi, message);
            }
        }
    }

    void encoderEvent(const Encoder::Event &event) {
        if (event == Encoder::Down) {
            setMode(Mode((int(_mode) + 1) % int(Mode::Last)));
            return;
        }

        switch (_mode) {
        case Mode::Buttons:
            switch (event) {
            case Encoder::Left:
                print("encoder left");
                break;
            case Encoder::Right:
                print("encoder right");
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    void buttonEvent(const ButtonLedMatrix::Event &event) {
        bool keyDown = event.action() == ButtonLedMatrix::Event::KeyDown;
        bool keyUp = event.action() == ButtonLedMatrix::Event::KeyUp;
        int index = event.value();

        switch (_mode) {
        case Mode::Buttons:
            if (keyDown) {
                print(FixedStringBuilder<16>("button %d down", index));
            } else if (keyUp) {
                print(FixedStringBuilder<16>("button %d up", index));
            }
            break;
        case Mode::Leds:
            if (keyDown) {
                if (index < 32) {
                    _leds[index] = LEDColor((int(_leds[index]) + 1) % int(LEDColor::Last));
                } else if (index == 32) {
                    fillLeds(LEDColor::Off);
                } else if (index == 33) {
                    fillLeds(LEDColor::Red);
                } else if (index == 34) {
                    fillLeds(LEDColor::Green);
                } else if (index == 35) {
                    fillLeds(LEDColor::Orange);
                }
            }
            break;
        case Mode::Midi:
            if (keyDown) {
                auto message = MidiMessage::makeNoteOn(0, 60 + index, 127);
                print(FixedStringBuilder<64>("send: note on, ch=%d, note=%d, vel=%d", message.channel(), message.note(), message.velocity()));
                midiSend(message);
            } else if (keyUp) {
                auto message = MidiMessage::makeNoteOff(0, 60 + index);
                print(FixedStringBuilder<64>("send: note off, ch=%d, note=%d, vel=%d", message.channel(), message.note(), message.velocity()));
                midiSend(message);
            }
            break;
        case Mode::Clock:
            if (index == 32) {
                _clockOutput = keyDown;
            } else if (index == 33) {
                _resetOutput = keyDown;
            }
            break;
        case Mode::CvOutputs:
            if (keyDown && index >= 16 && index < 24) {
                _cvOutputs[index - 16] = (_cvOutputs[index - 16] + 6) % 11 - 5;
            }
            break;
        case Mode::GateOutputs:
            if (index >= 16 && index < 24) {
                _gateOutputs[index - 16] = keyDown;
            }
            break;
        default:
            break;
        }
    }

    void midiReceive(MidiPort port, const MidiMessage &message) {
        if (_mode == Mode::Midi) {
            FixedStringBuilder<64> str;

            str("recv: ");

            if (message.isNoteOn() || message.isNoteOff()) {
                if (message.isNoteOn()) {
                    str("note on, ");
                } else if (message.isNoteOff()) {
                    str("note off, ");
                }
                str("ch=%d, note=%d, vel=%d", message.channel(), message.note(), message.velocity());
            } else if (message.isKeyPressure()) {
                str("key pressure, ch=%d, val=%d", message.channel(), message.keyPressure());
            } else if (message.isPitchBend()) {
                str("pitch bend, ch=%d, val=%d", message.channel(), message.pitchBend());
            } else if (message.isControlChange()) {
                str("cc, ch=%d, num=%d, val=%d", message.channel(), message.controlNumber(), message.controlValue());
            } else if (message.isProgramChange()) {
                str("program change, ch=%d, val=%d", message.channel(), message.programNumber());
            } else if (message.isChannelPressure()) {
                str("channel pressure, ch=%d, val=%d", message.channel(), message.channelPressure());
            } else {
                return;
            }

            print(str);
        }
    }

    void midiSend(const MidiMessage &message) {
        midi.send(message);
        usbMidi.send(0, message);
    }

    void setMode(Mode mode) {
        _mode = mode;
        clear();
        fillLeds();

        switch (_mode) {
        case Mode::Intro:
            print("welcome to the tester application");
            print("press encoder to switch between modes");
            break;
        case Mode::Buttons:
            print("press buttons and rotate encoder");
            break;
        case Mode::Leds:
            print("press buttons to toggle leds");
            print("press F1 to set all leds off");
            print("press F2 to set all leds red");
            print("press F3 to set all leds green");
            print("press F4 to set all leds orange");
            break;
        case Mode::Midi:
            print("press buttons to send midi notes");
            break;
        case Mode::Clock:
            _clockOutput = false;
            _resetOutput = false;
            break;
        case Mode::CvOutputs:
            _cvOutputs.fill(0);
            break;
        case Mode::GateOutputs:
            _gateOutputs.fill(false);
            break;
        default:
            break;
        }

        _lastRefresh = os::ticks();
        refresh();
    }

    void refresh() {
        switch (_mode) {
        case Mode::Clock:
            clear(false);
            print("press F1 to raise clock output");
            print("press F2 to raise reset output");
            print(FixedStringBuilder<32>("clock out: %s reset out: %s  ", _clockOutput ? "high" : "low", _resetOutput ? "high" : "low"));
            print(FixedStringBuilder<32>("clock in: %s reset in: %s  ", dio.clockInput.get() ? "high" : "low", dio.resetInput.get() ? "high" : "low"));
            break;
        case Mode::CvInputs:
            clear(false);
            for (int i = 0; i < 4; ++i) {
                uint16_t value = adc.channel(i);
                float volts = 5.f - value / 6553.5f;
                print(FixedStringBuilder<32>("cv%d: %.1fV (%d)  ", i + 1, volts, value));
            }
            break;
        case Mode::CvOutputs:
            clear(false);
            print("press track buttons to change cv outputs");
            for (int i = 0; i < 4; ++i) {
                int i1 = i * 2;
                int i2 = i * 2 + 1;
                print(FixedStringBuilder<32>("cv%d: %d.0V cv%d: %d.0V  ", i1 + 1, _cvOutputs[i1], i2 + 1, _cvOutputs[i2]));
            }
            break;
        case Mode::GateOutputs:
            clear(false);
            print("press track buttons to raise gate outputs");
            for (int i = 0; i < 4; ++i) {
                int i1 = i * 2;
                int i2 = i * 2 + 1;
                print(FixedStringBuilder<32>("gate%d: %s gate%d: %s  ", i1 + 1, _gateOutputs[i1] ? "high" : "low", i2 + 1, _gateOutputs[i2] ? "high" : "low"));
            }
            break;
        default:
            break;
        }
    }

    void draw() {
        static const char *titles[] = {
            "TESTER",
            "BUTTONS",
            "LEDS",
            "MIDI",
            "CLOCK",
            "CV INPUTS",
            "CV OUTPUTS",
            "GATE OUTPUTS"
        };

        drawClear(_canvas);
        drawTitle(_canvas, titles[int(_mode)]);
        drawLog(_canvas);

        lcd.draw(_frameBufferData);
    }

    void drawClear(Canvas &canvas) {
        canvas.setBlendMode(BlendMode::Set);
        canvas.setColor(Color::None);
        canvas.fill();
    }

    void drawTitle(Canvas &canvas, const char *title) {
        canvas.setBlendMode(BlendMode::Set);
        canvas.setColor(Color::Bright);
        canvas.fillRect(0, 0, 256, 10);
        canvas.setBlendMode(BlendMode::Sub);
        canvas.setFont(Font::Tiny);
        canvas.setColor(Color::Bright);
        canvas.drawText(2, 7, title);
    }

    void drawLog(Canvas &canvas) {
        canvas.setBlendMode(BlendMode::Set);
        canvas.setFont(Font::Tiny);
        canvas.setColor(Color::Bright);

        int x = 2;
        int y = 18;
        for (size_t line = 0; line < _logBuffer.lines(); ++line) {
            canvas.drawText(x, y, _logBuffer.line(line));
            y += 10;
        }
    }

    void clear(bool clearTerminal = true) {
        _logBuffer.clear();
        if (clearTerminal) {
            Console::write("\033[2J");
        }
        Console::write("\033[H");
    }

    void resetCursor() {
        Console::write("\033[H");
    }

    void print(const char *text) {
        Console::write(text);
        Console::write("\n");

        _logBuffer.print(text);
    }

    void fillLeds(LEDColor color = LEDColor::Off) {
        _leds.fill(color);
    }

    void setLed(int index, LEDColor color) {
        _leds[index] = color;
    }

    void updateLeds() {
        for (size_t i = 0; i < _leds.size(); ++i) {
            LEDColor c = _leds[i];
            uint8_t red = (c == LEDColor::Red || c == LEDColor::Orange) ? 0xff : 0;
            uint8_t green = (c == LEDColor::Green || c == LEDColor::Orange) ? 0xff : 0;
            blm.setLed(i, red, green);
        }
    }

    void updateDio() {
        dio.clockOutput.set(_clockOutput);
        dio.resetOutput.set(_resetOutput);
    }

    void updateCvOutputs() {
        static constexpr float volts0 = 5.17f;
        static constexpr float volts1 = -5.25f;

        for (int i = 0; i < 8; ++i) {
            float volts = _cvOutputs[i];
            uint16_t value = std::max(0, std::min(0x7fff, (int((volts - volts0) / (volts1 - volts0) * 32768))));
            dac.setValue(i, value);;
        }
        dac.write();
    }

    void updateGateOutputs() {
        for (int i = 0; i < 8; ++i) {
            gateOutput.setGate(i, _gateOutputs[i]);
        }
        gateOutput.update();
    }

    Mode _mode = Mode(0);
    uint32_t _lastRefresh = 0;

    LogBuffer<5, 64> _logBuffer;

    std::array<LEDColor, 32> _leds;

    bool _clockOutput = false;
    bool _resetOutput = false;
    std::array<int, 8> _cvOutputs;
    std::array<bool, 8> _gateOutputs;

    uint8_t _frameBufferData[256 * 64];
    FrameBuffer8bit _frameBuffer;
    Canvas _canvas;
    float _brightness = 1.0;
};

static TesterTask tester;

static os::PeriodicTask<1024> driverTask("driver", 2, os::time::ms(1), [] () {
    shiftRegister.process();
    blm.process();
    encoder.process();
});

static os::PeriodicTask<2048> usbhTask("usbh", 1, os::time::ms(1), [] () {
    usbh.process();
});

static os::PeriodicTask<4096> uiTask("tester", 0, os::time::ms(1), [] () {
    tester.update();
});

int main(void) {
    System::init();
    Console::init();
    HighResolutionTimer::init();

    shiftRegister.init();
    clockTimer.init();
    blm.init();
    encoder.init();
    lcd.init();
    adc.init();
    dac.init();
    dio.init();
    gateOutput.init();
    midi.init();
    usbMidi.init();
    usbh.init();
    usbh.powerOn();
    sdCard.init();

    tester.init();

	os::startScheduler();
}
