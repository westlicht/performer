#include "PerformerPage.h"

#include "Config.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"
#include "ui/layouts/GridLayout.h"
#include "ui/layouts/CenterLayout.h"

#include "core/utils/StringBuilder.h"

PerformerPage::PerformerPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void PerformerPage::enter() {
}

void PerformerPage::exit() {
}

void PerformerPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PERFORMER");

    const char *functionNames[] = { "MUTE", "UNMUTE", "CANCEL", nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    GridLayout<
        CONFIG_TRACK_COUNT, // CellCount
        16,                 // CellWidth
        16,                 // CellHeight
        4,                  // CellSpacing
        8                   // Padding
    > gridLayout;

    CenterLayout<
        Width,              // ContainerWidth
        Height,             // ContainerHeight
        gridLayout.width(), // Width
        gridLayout.height() // Height
    > centerLayout;

    constexpr int Border = 4;
    constexpr int BorderRequested = 6;

    const auto &playState = _project.playState();

    float globalMeasureFraction = _engine.globalMeasureFraction();
    bool hasRequested = false;

    canvas.setBlendMode(BlendMode::Set);

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &trackEngine = _engine.trackEngine(trackIndex);
        const auto &trackState = playState.trackState(trackIndex);

        int x = centerLayout.originX() + gridLayout.cellX(trackIndex);
        int y = centerLayout.originY() + gridLayout.cellY(trackIndex);
        int w = gridLayout.cellWidth();
        int h = gridLayout.cellHeight();

        canvas.setColor(trackEngine.gate() ? 0xf : 0x7);
        canvas.drawRect(x, y, w, h);
        canvas.setColor(0xf);
        if (trackState.mute() != trackState.requestedMute()) {
            hasRequested = true;
            canvas.fillRect(x + BorderRequested, y + BorderRequested, w - 2 * BorderRequested, h - 2 * BorderRequested);
        } else if (trackState.mute()) {
            canvas.fillRect(x + Border, y + Border, w - 2 * Border, h - 2 * Border);
        }
    }

    if (hasRequested) {
        canvas.setColor(0xf);
        canvas.hline(0, 10, globalMeasureFraction * Width);
    }
}

void PerformerPage::updateLeds(Leds &leds) {
}

void PerformerPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();
    auto &playState = _project.playState();

    if (key.isFunction()) {
        switch (key.function()) {
        case 0: playState.muteAll(key.shiftModifier() ? PlayState::Scheduled : PlayState::Immediate); break;
        case 1: playState.unmuteAll(key.shiftModifier() ? PlayState::Scheduled : PlayState::Immediate); break;
        case 2: playState.cancelScheduledMutesAndSolos(); break;
        }
    }

    if (key.isStep()) {
        if (key.step() < 8) {
            int track = key.step();
            playState.toggleMuteTrack(track, key.shiftModifier() ? PlayState::Scheduled : PlayState::Immediate);
        } else {
            int track = key.step() - 8;
            playState.soloTrack(track, key.shiftModifier() ? PlayState::Scheduled : PlayState::Immediate);
        }
    }
}

void PerformerPage::keyUp(KeyEvent &event) {
}

void PerformerPage::encoder(EncoderEvent &event) {
}
