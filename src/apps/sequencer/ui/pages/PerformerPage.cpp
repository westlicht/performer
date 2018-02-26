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

    const char *functionNames[] = { "MUTE", "UNMUTE", "FILL", nullptr, "CANCEL" };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    constexpr int Border = 4;
    constexpr int BorderRequested = 6;

    const auto &playState = _project.playState();

    float globalMeasureFraction = _engine.globalMeasureFraction();
    bool hasRequested = false;

    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &trackEngine = _engine.trackEngine(trackIndex);
        const auto &trackState = playState.trackState(trackIndex);

        int x = trackIndex * 32;
        int y = 16;

        int w = 16;
        int h = 16;

        x += 8;

        canvas.setColor(trackState.fill() ? 0xf : 0x7);
        canvas.drawTextCentered(x, y - 2, w, 8, FixedStringBuilder<8>("T%d", trackIndex + 1));

        y += 8;

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

    if (key.isFunction() && key.function() == 2) {
        playState.fillAll(true);
        event.consume();
    }

    if (key.isTrack()) {
        playState.fillTrack(key.track(), true);
        event.consume();
    }
}

void PerformerPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();
    auto &playState = _project.playState();

    if (key.isFunction() && key.function() == 2) {
        playState.fillAll(false);
        event.consume();
    }

    if (key.isTrack()) {
        playState.fillTrack(key.track(), false);
        event.consume();
    }
}

void PerformerPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &playState = _project.playState();

    if (key.isTrack()) {
        event.consume();
    }

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

void PerformerPage::encoder(EncoderEvent &event) {
}
