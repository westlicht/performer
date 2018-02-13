#include "TrackSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

TrackSetupPage::TrackSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void TrackSetupPage::enter() {
    _listModel.setTrack(_project.selectedTrack());
}

void TrackSetupPage::exit() {
}

void TrackSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, _listModel.track() != _project.selectedTrack() ? "COMMIT" : nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void TrackSetupPage::updateLeds(Leds &leds) {
}

void TrackSetupPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction() && key.function() == 4) {
        _project.setTrack(_project.selectedTrackIndex(), _listModel.track());
    }

    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
        _listModel.setTrack(_project.selectedTrack());
    }

    ListPage::keyDown(event);
}

void TrackSetupPage::keyUp(KeyEvent &event) {
    ListPage::keyUp(event);
}

void TrackSetupPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);
}
