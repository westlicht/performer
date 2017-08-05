#include "MainPage.h"

#include "ui/LedPainter.h"

MainPage::MainPage(PageManager &pageManager, Model &model, Engine &engine) :
    Page(pageManager),
    _model(model),
    _engine(engine),
    _project(model.project())
{}

void MainPage::enter() {
}

void MainPage::exit() {
}

void MainPage::draw(Canvas &canvas) {
    canvas.setColor(0);
    canvas.fill();

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &track = _engine.track(trackIndex);
        const auto &sequence = track.sequence();
        for (int step = 0; step < 16; ++step) {
            int x = step * 16;
            int y = trackIndex * 7 + 0;
            auto active = sequence.step(step).active;
            canvas.setColor(step == track.currentStep() ? (active ? 0xf : 0x7) : (active ? 0x7 : 0x3));
            if (_project.isSelectedTrack(trackIndex)) {
                canvas.setColor(15);
            }
            if (active) {
                canvas.fillRect(x + 1, y + 1, 16 - 2, 7 - 2);
            } else {
                canvas.drawRect(x + 1, y + 1, 16 - 2, 7 - 2);
            }
        }
    }
}

void MainPage::updateLeds(Leds &leds) {
    LedPainter::drawTracksGateAndSelected(leds, _engine, _project.selectedTrackIndex());

    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = track.sequence();
    LedPainter::drawSequenceGateAndCurrentStep(leds, sequence, track.currentStep());
}

void MainPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isTrack()) {
        // DBG("select track %d", key.track());
        _project.setSelectedTrackIndex(key.track());
        event.consume();
    }

    if (key.isStep()) {
        // DBG("toggle step %d", key.step());
        auto &sequence = _project.selectedSequence();
        sequence.step(key.step()).toggle();
        event.consume();
    }
}

void MainPage::keyUp(KeyEvent &event) {
    // event.consume();
}

void MainPage::encoder(EncoderEvent &event) {
    // event.consume();
}
