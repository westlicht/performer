#include "TopPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"

TopPage::TopPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void TopPage::init() {
    setMode(Mode::Project);
}

void TopPage::updateLeds(Leds &leds) {
    leds.set(
        Key::Start,
        _engine.running() && _engine.tick() % CONFIG_PPQN < (CONFIG_PPQN / 8),
        false // _engine.running()
    );

    if (_keyState[Key::Page]) {
        LedPainter::drawSelectedPage(leds, _mode);
    } else {
        LedPainter::drawTracksGateAndSelected(leds, _engine, _project.selectedTrackIndex());
    }
}

void TopPage::keyDown(KeyEvent &event) {
    event.consume();
}

void TopPage::keyUp(KeyEvent &event) {
    event.consume();
}

void TopPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
        if (isSequencePage()) {
            setSequencePage();
        } else if (isSequenceSetupPage()) {
            setSequenceSetupPage();
        }
    }

    if (key.isPageSelect()) {
        setMode(Mode(key.pageSelect()));
    }

    if (key.is(Key::Start)) {
        if (key.shiftModifier()) {
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
            // bpm page
            _manager.pages().bpm.show();
        }
    }

    event.consume();
}

void TopPage::encoder(EncoderEvent &event) {
    event.consume();
}

void TopPage::setMode(Mode mode) {
    switch (mode) {
    case Mode::Project:
        setMainPage(_manager.pages().project);
        break;
    case Mode::Pattern:
        setMainPage(_manager.pages().pattern);
        break;
    case Mode::TrackSetup:
        setMainPage(_manager.pages().trackSetup);
        break;
    case Mode::Sequence:
        setSequencePage();
        break;
    case Mode::SequenceSetup:
        setSequenceSetupPage();
        break;
    case Mode::Performer:
        setMainPage(_manager.pages().performer);
        break;
    case Mode::Monitor:
        setMainPage(_manager.pages().monitor);
    default:
        return;
    }
    _mode = mode;
}

void TopPage::setMainPage(Page &page) {
    _manager.reset(&_manager.pages().top);
    _manager.push(&page);
}

bool TopPage::isSequencePage() {
    return
        _manager.top() == &_manager.pages().noteSequence ||
        _manager.top() == &_manager.pages().curveSequence;
}

void TopPage::setSequencePage() {
    switch (_project.selectedTrackSetup().trackMode()) {
    case Types::TrackMode::Note:
        setMainPage(_manager.pages().noteSequence);
        break;
    case Types::TrackMode::Curve:
        setMainPage(_manager.pages().curveSequence);
        break;
    case Types::TrackMode::Last:
        break;
    }
}

bool TopPage::isSequenceSetupPage() {
    return
        _manager.top() == &_manager.pages().noteSequenceSetup ||
        _manager.top() == &_manager.pages().curveSequenceSetup;
}

void TopPage::setSequenceSetupPage() {
    switch (_project.selectedTrackSetup().trackMode()) {
    case Types::TrackMode::Note:
        setMainPage(_manager.pages().noteSequenceSetup);
        break;
    case Types::TrackMode::Curve:
        setMainPage(_manager.pages().curveSequenceSetup);
        break;
    case Types::TrackMode::Last:
        break;
    }
}
