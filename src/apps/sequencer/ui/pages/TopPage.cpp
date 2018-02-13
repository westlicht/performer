#include "TopPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"

TopPage::TopPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void TopPage::updateLeds(Leds &leds) {
    leds.set(
        Key::Start,
        _engine.running() && _engine.tick() % CONFIG_PPQN < (CONFIG_PPQN / 8),
        false // _engine.running()
    );

    LedPainter::drawTracksGateAndSelected(leds, _engine, _project.selectedTrackIndex());
}

void TopPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
    }

    if (key.isModeSelect()) {
        switch (key.modeSelect()) {
        case 0: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().project); break;
        case 1: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().pattern); break;
        case 2: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().trackSetup); break;
        case 3: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().noteSequence); break;
        case 4: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().curveSequence); break;
        case 5: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().sequenceSetup); break;
        case 6: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().performer); break;
        case 7: _manager.reset(&_manager.pages().top); _manager.push(&_manager.pages().textInput); break;
        }
    }

    if (key.is(Key::Start)) {
        if (_keyState[Key::BPM]) {
            // tap tempo
            _engine.tapTempoTap();
        } else if (key.shiftModifier()) {
            // restart
            _engine.start();
        } else {
            // start/stop
            if (!_engine.running()) {
                _engine.resume();
            } else {
                _engine.stop();
            }
        }
        event.consume();
    }

    if (key.is(Key::BPM)) {
        if (key.shiftModifier()) {
            // clock setup page
            _manager.pages().clockSetup.show();
        } else {
            // bpm value
            _engine.tapTempoReset();
            _manager.pages().value.show({
                Key::None,
                [this] (StringBuilder &string) {
                    string("BPM: %.1f", _model.project().bpm());
                },
                [this] (int encoderValue, bool encoderShift, bool keyShift) {
                    _model.project().setBpm(_model.project().bpm() + encoderValue * (encoderShift ? 0.1f : 1.f) * (keyShift ? 10.f : 1.f));
                }
            });
        }
    }

    if (key.is(Key::Mute)) {
        _manager.pages().mute.show();
    }

    event.consume();
}

void TopPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.is(Key::BPM)) {
        _manager.pages().value.close();
    }

    event.consume();
}

void TopPage::encoder(EncoderEvent &event) {
    event.consume();
}
