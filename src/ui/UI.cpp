#include "UI.h"
#include "Keys.h"

#include "core/Debug.h"
#include "core/profiler/Profiler.h"

PROFILER_INTERVAL(UIDraw, "UI.draw")
PROFILER_INTERVAL(UIInterval, "UI.interval")

UI::UI(Engine &engine, LCD &lcd, ButtonLedMatrix &blm) :
    _engine(engine),
    _lcd(lcd),
    _blm(blm),
    _frameBuffer(LCD_WIDTH, LCD_HEIGHT, _frameBufferData),
    _canvas(_frameBuffer)
{}

void UI::init() {
    _canvas.drawText(10, 30, "Hello World!");
    _lcd.draw(_frameBuffer.data());
}

void UI::update() {
    handleKeys();
    updateLeds();

    static int counter = 0;
    if (counter % 20 == 0) {
        PROFILER_INTERVAL_END(UIInterval);
        PROFILER_INTERVAL_BEGIN(UIInterval);
        PROFILER_INTERVAL_BEGIN(UIDraw);

        // _canvas.setColor(0);
        // _canvas.fill();
        // _canvas.setColor(0xff);
        // _canvas.drawText(10 + (counter / 10) % 100, 30, "Hello World!");

        _canvas.setColor(0);
        _canvas.fill();

        int currentStep = (_engine.clock().tick() / (192 / 4)) % 16;
        for (int track = 0; track < 8; ++track) {
            const auto &sequence = _engine.track(track).sequence();
            for (int step = 0; step < 16; ++step) {
                int x = step * 16;
                int y = track * 7 + 0;
                auto active = sequence.step(step).active;
                _canvas.setColor(step == currentStep ? (active ? 0xf : 0x7) : (active ? 0x7 : 0x3));
                if (track == _selectedTrack) {
                    _canvas.setColor(15);
                }
                if (active) {
                    _canvas.fillRect(x + 1, y + 1, 16 - 2, 7 - 2);
                } else {
                    _canvas.drawRect(x + 1, y + 1, 16 - 2, 7 - 2);
                }
            }
        }

        _canvas.setColor(0xf);
        _canvas.setFont(Font::Tiny);
        _canvas.drawText(10, 64-1, "this is a test with tiny 5x5");

        _lcd.draw(_frameBuffer.data());

        PROFILER_INTERVAL_END(UIDraw);
    }
    ++counter;
}

void UI::handleKeys() {
    ButtonLedMatrix::Event event;
    while (_blm.nextEvent(event)) {
        switch (event.action()) {
        case ButtonLedMatrix::KeyDown:
            // DBG("down %d", event.key());
            keyDown(Key(event.value()));
            break;
        case ButtonLedMatrix::KeyUp:
            // DBG("up %d", event.key());
            keyUp(Key(event.value()));
            break;
        case ButtonLedMatrix::Encoder:
            encoder(event.value());
            break;
        }
    }
}

void UI::keyDown(const Key &key) {
    if (key.is(Key::Start)) {
        if (!_engine.running()) {
            _engine.start();
        } else {
            _engine.stop();
        }
    }

    if (key.isTrack()) {
        DBG("select track %d", key.track());
        _selectedTrack = key.track();
    }

    if (key.isStep()) {
        DBG("toggle step %d", key.step());
        auto &sequence = _engine.track(_selectedTrack).sequence();
        sequence.step(key.step()).toggle();

    }
}

void UI::keyUp(const Key &key) {
}

void UI::encoder(int value) {
}

void UI::updateLeds() {
    _blm.setLed(Key::Start, _engine.running() ? 0xff : 0, 0);

    for (int track = 0; track < 8; ++track) {
        _blm.setLed(KeyCode::Track(track), 0, track == _selectedTrack ? 0xff : 0);
    }

    int currentStep = (_engine.clock().tick() / (192 / 4)) % 16;
    const auto &sequence = _engine.track(_selectedTrack).sequence();
    for (int step = 0; step < 16; ++step) {
        _blm.setLed(KeyCode::Step(step), step == currentStep ? 0xff : 0, sequence.step(step).active ? 0xff : 0);
    }
}
