#include "TopPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"

TopPage::TopPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void TopPage::init() {
    setMode(Mode::Project);
}

void TopPage::initRoute(Routing::Param param, int trackIndex) {
    int routeIndex = _project.routing().firstEmptyRouteIndex();
    if (routeIndex >= 0) {
        Routing::Route initRoute;
        initRoute.setParam(param);
        // initRoute.setTrack(trackIndex);
        setMode(Mode::Routing);
        _manager.pages().route.show(routeIndex, initRoute);
    } else {
        showMessage("All routes are used!");
    }
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
        setMode(_mode);
        event.consume();
    }

    if (key.pageModifier() && key.is(Key::Left)) {
        switchMode(-1);
        event.consume();
    }
    if (key.pageModifier() && key.is(Key::Right)) {
        switchMode(1);
        event.consume();
    }

    if (key.isPageSelect()) {
        setMode(Mode(key.pageSelect()));
        event.consume();
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
    auto &pages = _manager.pages();

    switch (mode) {
    case Mode::Project:
        setMainPage(pages.project);
        break;
    case Mode::Layout:
        setMainPage(pages.layout);
        break;
    case Mode::Track:
        setMainPage(pages.track);
        break;
    case Mode::Sequence:
        setSequencePage();
        break;
    case Mode::SequenceSetup:
        setSequenceSetupPage();
        break;
    case Mode::Pattern:
        setMainPage(pages.pattern);
        break;
    case Mode::Performer:
        setMainPage(pages.performer);
        break;
    case Mode::Scale:
        setSequenceSetupPage(0);
        break;
    case Mode::Divisor:
        setSequenceSetupPage(1);
        break;
    case Mode::ResetMeasure:
        setSequenceSetupPage(2);
        break;
    case Mode::PlayMode:
        setSequenceSetupPage(3);
        break;
    case Mode::FirstStep:
        setSequenceSetupPage(4);
        break;

    case Mode::Euclidean:
        setMainPage(pages.euclidean);
        break;
    case Mode::Routing:
        setMainPage(pages.routing);
        break;
    case Mode::Monitor:
        setMainPage(pages.monitor);
        break;
    default:
        return;
    }
    _mode = mode;
}

void TopPage::switchMode(int direction) {
    int newMode = _mode;
    while ((direction > 0 && ++newMode < int(Mode::Last)) || (direction < 0 && --newMode >= 0)) {
        setMode(Mode(newMode));
        if (_mode == Mode(newMode)) {
            break;
        }
    }
}

void TopPage::setMainPage(Page &page) {
    _manager.reset(&_manager.pages().top);
    _manager.push(&page);
}

void TopPage::setSequencePage() {
    auto &pages = _manager.pages();

    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        setMainPage(pages.noteSequence);
        break;
    case Track::TrackMode::Curve:
        setMainPage(pages.curveSequence);
        break;
    case Track::TrackMode::MidiCv:
        setMainPage(pages.track);
        break;
    case Track::TrackMode::Last:
        break;
    }
}

void TopPage::setSequenceSetupPage() {
    auto &pages = _manager.pages();

    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        setMainPage(pages.noteSequenceSetup);
        break;
    case Track::TrackMode::Curve:
        setMainPage(pages.curveSequenceSetup);
        break;
    case Track::TrackMode::MidiCv:
        setMainPage(pages.track);
        break;
    case Track::TrackMode::Last:
        break;
    }
}

void TopPage::setSequenceSetupPage(int row) {
    auto &pages = _manager.pages();

    pages.noteSequenceSetup.setSelectedRow(row);
    pages.noteSequenceSetup.setEdit(true);
    pages.curveSequenceSetup.setSelectedRow(row);
    pages.curveSequenceSetup.setEdit(true);

    setSequenceSetupPage();
}
