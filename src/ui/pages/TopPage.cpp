#include "TopPage.h"

#include "Pages.h"

TopPage::TopPage(PageManager &pageManager, PageContext &context) :
    Page(pageManager),
    _model(context.model),
    _engine(context.engine)
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
        auto &pm = _pageManager;
        switch (key.track()) {
        case 0: pm.reset(&pm.pages().topPage); pm.push(&pm.pages().mainPage); break;
        case 1: pm.reset(&pm.pages().topPage); pm.push(&pm.pages().trackPage); break;
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
        _pageManager.pages().valuePage.show({
            Key::BPM,
            [this] (StringBuilder<32> &string) { string("BPM: %.1f", _model.project().bpm()); },
            [this] (int value) { _model.project().setBpm(_model.project().bpm() + value * 0.1f); }
        });
    }

    if (key.is(Key::Mute)) {
        _pageManager.pages().mutePage.show();
    }

    event.consume();
}

void TopPage::keyUp(KeyEvent &event) {
    event.consume();
}

void TopPage::encoder(EncoderEvent &event) {
    event.consume();
}
