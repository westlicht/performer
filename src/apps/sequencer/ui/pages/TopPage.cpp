#include "TopPage.h"

#include "Pages.h"

TopPage::TopPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void TopPage::updateLeds(Leds &leds) {
    leds.set(
        Key::Start,
        _engine.running() && _engine.tick() % (CONFIG_PPQN / 2) < (CONFIG_PPQN / 8),
        false // _engine.running()
    );
}

void TopPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.shiftModifier() && key.isTrack()) {
        switch (key.track()) {
        case 0: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().main); break;
        case 1: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().track); break;
        case 2: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().sequenceSetup); break;
        }
    }

    if (key.is(Key::Start)) {
        if (key.shiftModifier()) {
            _engine.start();
        } else {
            if (!_engine.running()) {
                _engine.resume();
            } else {
                _engine.stop();
            }
        }
        event.consume();
    }

    if (key.is(Key::BPM)) {
        _manager.pages().valuePage.show({
            Key::BPM,
            [this] (StringBuilder &string) {
                string("BPM: %.1f", _model.project().bpm());
            },
            [this] (int encoderValue, bool encoderShift, bool keyShift) {
                _model.project().setBpm(_model.project().bpm() + encoderValue * (encoderShift ? 1.f : 0.1f) * (keyShift ? 10.f : 1.f));
            }
        });
    }

    if (key.is(Key::Mute)) {
        _manager.pages().mute.show();
    }

    event.consume();
}

void TopPage::keyUp(KeyEvent &event) {
    event.consume();
}

void TopPage::encoder(EncoderEvent &event) {
    event.consume();
}
