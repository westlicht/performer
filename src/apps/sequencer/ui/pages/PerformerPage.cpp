#include "PerformerPage.h"

#include "Config.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"
#include "ui/painters/SequencePainter.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Latch   = 0,
    Sync    = 1,
    Unmute  = 2,
    Fill    = 3,
    Cancel  = 4
};

PerformerPage::PerformerPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void PerformerPage::enter() {
    _latching = false;
    _syncing = false;
}

void PerformerPage::exit() {
    _project.playState().commitLatchedRequests();
}

void PerformerPage::draw(Canvas &canvas) {
    const auto &playState = _project.playState();
    bool hasCancel = playState.hasSyncedRequests() || playState.hasLatchedRequests();
    const char *functionNames[] = { "LATCH", "SYNC", "UNMUTE", "FILL", hasCancel ? "CANCEL" : nullptr };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PERFORMER");
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    constexpr int Border = 4;
    constexpr int BorderRequested = 6;

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

        // draw track number (highlight when fill is active)
        canvas.setColor(trackState.fill() ? Color::Bright : Color::Medium);
        canvas.drawTextCentered(x, y - 2, w, 8, FixedStringBuilder<8>("T%d", trackIndex + 1));

        y += 8;

        // draw outer rectangle (track activity)
        canvas.setColor(trackEngine.activity() ? Color::Bright : Color::Medium);
        canvas.drawRect(x, y, w, h);

        // draw mutes and mute requests
        canvas.setColor(Color::Bright);
        if (trackState.hasMuteRequest() && trackState.mute() != trackState.requestedMute()) {
            hasRequested = true;
            canvas.fillRect(x + BorderRequested, y + BorderRequested, w - 2 * BorderRequested, h - 2 * BorderRequested);
        } else if (trackState.mute()) {
            canvas.fillRect(x + Border, y + Border, w - 2 * Border, h - 2 * Border);
        }

        // draw sequence progress
        SequencePainter::drawSequenceProgress(canvas, x, y + h + 2, w, 2, trackEngine.sequenceProgress());

        // draw fill & fill amount amount
        bool pressed = pageKeyState()[MatrixMap::fromStep(trackIndex)];
        canvas.setColor(pressed ? Color::Medium : Color::Low);
        canvas.fillRect(x, y + h + 6, w, 4);
        canvas.setColor(pressed ? Color::Bright : Color::Medium);
        canvas.fillRect(x, y + h + 6, (trackState.fillAmount() * w) / 100, 4);
    }

    if (playState.hasSyncedRequests() && hasRequested) {
        canvas.setColor(Color::Bright);
        canvas.hline(0, 10, _engine.syncFraction() * Width);
    }
}

void PerformerPage::updateLeds(Leds &leds) {
    const auto &playState = _project.playState();

    LedPainter::drawTrackGates(leds, _engine, _project.playState());

    uint8_t activeFills = 0;
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &trackState = playState.trackState(trackIndex);
        activeFills |= trackState.fill() ? (1<<trackIndex) : 0;
    }

    LedPainter::drawMutes(leds, 0, 0);
    LedPainter::drawFills(leds, activeFills);
}

void PerformerPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Latch:
            _latching = true;
            break;
        case Function::Sync:
            _syncing = true;
            break;
        case Function::Fill:
            updateFills();
            break;
        default:
            break;
        }
        event.consume();
    }

    if (key.isStep()) {
        updateFills();
        event.consume();
    }
}

void PerformerPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    bool closePage = false;

    if (key.isPerformer()) {
        closePage = true;
        event.consume();
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Latch:
            closePage = true;
            _latching = false;
            _project.playState().commitLatchedRequests();
            break;
        case Function::Sync:
            closePage = true;
            _syncing = false;
            break;
        case Function::Fill:
            updateFills();
            break;
        default:
            break;
        }
        event.consume();
    }

    if (key.isStep()) {
        closePage = true;
        updateFills();
        event.consume();
    }

    bool stepKeyPressed = false;
    for (int step = 0; step < 16; ++step) {
        stepKeyPressed |= pageKeyState()[MatrixMap::fromStep(step)];
    }

    bool canClose = _modal && !_latching && !_syncing && !globalKeyState()[Key::Performer] && !stepKeyPressed;
    if (canClose && closePage) {
        close();
    }
}

void PerformerPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &playState = _project.playState();

    if (key.pageModifier()) {
        return;
    }

    // use immediate by default
    // use latched when LATCH is pressed
    // use synced when SYNC is pressed
    PlayState::ExecuteType executeType = _latching ? PlayState::Latched : (_syncing ? PlayState::Synced : PlayState::Immediate);

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Latch:
            break;
        case Function::Sync:
            break;
        case Function::Unmute:
            playState.unmuteAll(executeType);
            break;
        case Function::Fill:
            updateFills();
            break;
        case Function::Cancel:
            playState.cancelMuteRequests();
            break;
        }
        event.consume();
    }

    if (key.isTrackSelect()) {
        if (key.shiftModifier()) {
            playState.soloTrack(key.track(), executeType);
        } else {
            playState.toggleMuteTrack(key.track(), executeType);
        }
        event.consume();
    }
}

void PerformerPage::encoder(EncoderEvent &event) {
    for (int trackIndex = 0; trackIndex < 8; ++trackIndex) {
        if (pageKeyState()[MatrixMap::fromStep(trackIndex)]) {
            _project.playState().trackState(trackIndex).editFillAmount(event.value(), false);
        }
    }
}

void PerformerPage::updateFills() {
    auto &playState = _project.playState();
    bool fillPressed = pageKeyState()[MatrixMap::fromFunction(int(Function::Fill))];
    bool holdPressed = pageKeyState()[Key::Shift];

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        bool trackFill = pageKeyState()[MatrixMap::fromStep(8 + trackIndex)];
        playState.fillTrack(trackIndex, trackFill || fillPressed, holdPressed);
    }
}
