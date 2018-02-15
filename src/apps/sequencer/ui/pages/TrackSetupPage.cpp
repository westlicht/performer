#include "TrackSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

TrackSetupPage::TrackSetupPage(PageManager &manager, PageContext &context) :
    ListPage(manager, context, _listModel)
{}

void TrackSetupPage::enter() {
    _listModel.setTrackSetup(_project.selectedTrackSetup());
}

void TrackSetupPage::exit() {
}

void TrackSetupPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "TRACK SETUP");

    const char *functionNames[] = { nullptr, nullptr, nullptr, nullptr, _listModel.trackSetup() != _project.selectedTrackSetup() ? "COMMIT" : nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    ListPage::draw(canvas);
}

void TrackSetupPage::updateLeds(Leds &leds) {
}

void TrackSetupPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction() && key.function() == 4) {
        _project.setTrackSetup(_project.selectedTrackIndex(), _listModel.trackSetup());
    }

    if (key.isTrackSelect()) {
        _project.setSelectedTrackIndex(key.trackSelect());
        _listModel.setTrackSetup(_project.selectedTrackSetup());
    }

    ListPage::keyDown(event);
}

void TrackSetupPage::keyUp(KeyEvent &event) {
    ListPage::keyUp(event);
}

void TrackSetupPage::encoder(EncoderEvent &event) {
    ListPage::encoder(event);
}
