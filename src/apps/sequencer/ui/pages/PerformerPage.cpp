#include "PerformerPage.h"

#include "Config.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

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
    resetKeyState();

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
    WindowPainter::drawFooter(canvas, functionNames, keyState());

    constexpr int Border = 4;
    constexpr int BorderRequested = 6;

    float syncMeasureFraction = _engine.syncMeasureFraction();
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

        canvas.setColor(trackEngine.activity() ? 0xf : 0x7);
        canvas.drawRect(x, y, w, h);

        canvas.setColor(0xf);
        if (trackState.hasMuteRequest() && trackState.mute() != trackState.requestedMute()) {
            hasRequested = true;
            canvas.fillRect(x + BorderRequested, y + BorderRequested, w - 2 * BorderRequested, h - 2 * BorderRequested);
        } else if (trackState.mute()) {
            canvas.fillRect(x + Border, y + Border, w - 2 * Border, h - 2 * Border);
        }
    }

    if (playState.hasSyncedRequests() && hasRequested) {
        canvas.setColor(0xf);
        canvas.hline(0, 10, syncMeasureFraction * Width);
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
        updateFills();
        event.consume();
    }

    bool canClose = _modal && !_latching && !_syncing && !globalKeyState()[Key::Performer];
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

    if (key.isTrackSelect()) {
        event.consume();
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
        playState.toggleMuteTrack(key.track(), executeType);
        event.consume();
    } else if (key.isStep() && key.step() < 8) {
        playState.soloTrack(key.step(), executeType);
        event.consume();
    }
}

void PerformerPage::encoder(EncoderEvent &event) {
}

void PerformerPage::updateFills() {
    auto &playState = _project.playState();
    bool fillPressed = keyState()[MatrixMap::fromFunction(int(Function::Fill))];

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        bool trackFill = keyState()[MatrixMap::fromStep(8 + trackIndex)];
        playState.fillTrack(trackIndex, trackFill || fillPressed);
    }
}
