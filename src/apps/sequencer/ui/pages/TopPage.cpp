#include "TopPage.h"

#include "Pages.h"

#include "ui/model/NoteSequenceListModel.h"
#include "ui/model/CurveSequenceListModel.h"

#include "ui/LedPainter.h"

TopPage::TopPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void TopPage::init() {
    setMode(Mode::Project);
}

void TopPage::editRoute(Routing::Param param, int trackIndex) {
    if (param == Routing::Param::None) {
        return;
    }

    int routeIndex = _project.routing().findRoute(param, trackIndex);
    if (routeIndex >= 0) {
        setMode(Mode::Routing);
        _manager.pages().route.show(routeIndex);
        return;
    }

    routeIndex = _project.routing().findEmptyRoute();
    if (routeIndex >= 0) {
        Routing::Route initRoute;
        initRoute.setParam(param);
        initRoute.setTracks(1<<trackIndex);
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
        if (key.pageModifier()) {
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
    case Mode::SequenceEdit:
        setSequenceEditPage();
        break;
    case Mode::Pattern:
        setMainPage(pages.pattern);
        break;
    case Mode::Performer:
        setMainPage(pages.performer);
        break;

    case Mode::Scale:
    case Mode::RootNote:
    case Mode::Divisor:
    case Mode::ResetMeasure:
    case Mode::RunMode:
    case Mode::FirstStep:
    case Mode::LastStep:
        setSequencePage(mode);
        break;

    case Mode::Routing:
        setMainPage(pages.routing);
        break;
    case Mode::UserScale:
        setMainPage(pages.userScale);
        break;
    case Mode::Monitor:
        setMainPage(pages.monitor);
        break;
    case Mode::Settings:
        setMainPage(pages.settings);
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

void TopPage::setSequencePage(bool edit) {
    auto &pages = _manager.pages();

    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        pages.noteSequence.setEdit(edit);
        setMainPage(pages.noteSequence);
        break;
    case Track::TrackMode::Curve:
        pages.curveSequence.setEdit(edit);
        setMainPage(pages.curveSequence);
        break;
    case Track::TrackMode::MidiCv:
        setMainPage(pages.track);
        break;
    case Track::TrackMode::Last:
        break;
    }
}

void TopPage::setSequencePage(Mode mode) {
    auto &pages = _manager.pages();

    auto setSequenceParameter = [&pages] (
        NoteSequenceListModel::Item noteSequenceItem,
        CurveSequenceListModel::Item curveSequenceItem)
    {
        pages.noteSequence.setSelectedRow(int(noteSequenceItem));
        pages.curveSequence.setSelectedRow(int(curveSequenceItem));
    };

    switch (mode) {
    case Mode::Scale:
        setSequenceParameter(NoteSequenceListModel::Item::Scale, CurveSequenceListModel::Item::Range);
        break;
    case Mode::RootNote:
        setSequenceParameter(NoteSequenceListModel::Item::RootNote, CurveSequenceListModel::Item::Range);
        break;
    case Mode::Divisor:
        setSequenceParameter(NoteSequenceListModel::Item::Divisor, CurveSequenceListModel::Item::Divisor);
        break;
    case Mode::ResetMeasure:
        setSequenceParameter(NoteSequenceListModel::Item::ResetMeasure, CurveSequenceListModel::Item::ResetMeasure);
        break;
    case Mode::RunMode:
        setSequenceParameter(NoteSequenceListModel::Item::RunMode, CurveSequenceListModel::Item::RunMode);
        break;
    case Mode::FirstStep:
        setSequenceParameter(NoteSequenceListModel::Item::FirstStep, CurveSequenceListModel::Item::FirstStep);
        break;
    case Mode::LastStep:
        setSequenceParameter(NoteSequenceListModel::Item::LastStep, CurveSequenceListModel::Item::LastStep);
        break;
    default:
        setSequencePage();
        return;
    }

    setSequencePage(true);
}

void TopPage::setSequenceEditPage() {
    auto &pages = _manager.pages();

    switch (_project.selectedTrack().trackMode()) {
    case Track::TrackMode::Note:
        setMainPage(pages.noteSequenceEdit);
        break;
    case Track::TrackMode::Curve:
        setMainPage(pages.curveSequenceEdit);
        break;
    case Track::TrackMode::MidiCv:
        setMainPage(pages.track);
        break;
    case Track::TrackMode::Last:
        break;
    }
}
