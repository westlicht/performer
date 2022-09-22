#include "PatternPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "model/PlayState.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Latch       = 0,
    Sync        = 1,
    SnapRevert  = 2,
    SnapCommit  = 3,
    Cancel      = 4,
};

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Duplicate,
    Save,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUP"},
    { "SAVE PR."},
};


PatternPage::PatternPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void PatternPage::enter() {
    _latching = false;
    _syncing = false;
}

void PatternPage::exit() {
    _project.playState().commitLatchedRequests();
}

void PatternPage::draw(Canvas &canvas) {
    const auto &playState = _project.playState();
    bool hasCancel = playState.hasSyncedRequests() || playState.hasLatchedRequests();
    bool snapshotActive = playState.snapshotActive();
    const char *functionNames[] = {
        "LATCH",
        "SYNC",
        snapshotActive ? "REVERT" : "SNAP",
        snapshotActive ? "COMMIT" : nullptr,
        hasCancel ? "CANCEL" : nullptr
    };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PATTERN");
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    bool hasRequested = false;

    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &trackEngine = _engine.trackEngine(trackIndex);
        const auto &trackState = playState.trackState(trackIndex);
        bool trackSelected = pageKeyState()[MatrixMap::fromTrack(trackIndex)];

        int x = trackIndex * 32;
        int y = 16;

        int w = 28;
        int h = 10;

        x += 2;

        canvas.setColor(trackSelected ? Color::Bright : Color::Medium);
        canvas.drawTextCentered(x, y - 2, w, 8, FixedStringBuilder<8>("T%d", trackIndex + 1));

        y += 11;

        canvas.setColor(trackEngine.activity() ? Color::Bright : Color::Medium);
        canvas.drawRect(x, y, w, h);

        for (int p = 0; p < 16; ++p) {
            int px = x + (p % 8) * 3 + 2;
            int py = y + (p / 8) * 3 + 2;
            if (p == trackState.pattern()) {
                canvas.setColor(Color::Bright);
                canvas.fillRect(px, py, 3, 3);
            } else if (trackState.hasPatternRequest() && p == trackState.requestedPattern()) {
                canvas.setColor(Color::Medium);
                canvas.fillRect(px, py, 3, 3);
            } else {
                canvas.setColor(Color::Low);
                canvas.point(px + 1, py + 1);
            }
        }

        y += 5;

        canvas.setColor(trackSelected ? Color::Bright : Color::Medium);
        canvas.drawTextCentered(x, y + 10, w, 8, snapshotActive ? "S" : FixedStringBuilder<8>("P%d", trackState.pattern() + 1));

        if (trackState.hasPatternRequest() && trackState.pattern() != trackState.requestedPattern()) {
            hasRequested = true;
        }
    }

    if (playState.hasSyncedRequests() && hasRequested) {
        canvas.setColor(Color::Bright);
        canvas.hline(0, 10, _engine.syncFraction() * Width);
    }
}

void PatternPage::updateLeds(Leds &leds) {
    const auto &playState = _project.playState();

    if (playState.snapshotActive()) {
        LedPainter::drawSelectedPattern(leds, _snapshotTargetPattern, _snapshotTargetPattern);
    } else if (globalKeyState()[Key::Shift]) {
        LedPainter::drawSelectedPattern(leds, _project.selectedPatternIndex(), _project.selectedPatternIndex());
    } else {
        uint16_t allActivePatterns = 0;
        uint16_t allRequestedPatterns = 0;
        uint16_t selectedActivePatterns = 0;
        uint16_t selectedRequestedPatterns = 0;

        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            const auto &trackState = playState.trackState(trackIndex);
            bool hasPatternRequest = trackState.hasPatternRequest();
            int pattern = trackState.pattern();
            int requestedPattern = trackState.requestedPattern();
            allActivePatterns |= (pattern < 16) ? (1<<pattern) : 0;
            allRequestedPatterns |= (hasPatternRequest && requestedPattern < 16) ? (1<<requestedPattern) : 0;
            if (pageKeyState()[MatrixMap::fromTrack(trackIndex)]) {
                selectedActivePatterns |= (pattern < 16) ? (1<<pattern) : 0;
                selectedRequestedPatterns |= (hasPatternRequest && requestedPattern < 16) ? (1<<requestedPattern) : 0;
            }
        }

        if (selectedActivePatterns || selectedRequestedPatterns) {
            LedPainter::drawSelectedPatterns(leds, selectedActivePatterns, selectedRequestedPatterns);
        } else {
            LedPainter::drawSelectedPatterns(leds, allActivePatterns, allRequestedPatterns);
        }
    }
}

void PatternPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Latch:
            _latching = true;
            break;
        case Function::Sync:
            _syncing = true;
            break;
        default:
            break;
        }
        event.consume();
    }

    if (_project.playState().snapshotActive() && key.isStep()) {
        _snapshotTargetPattern = key.step();
        event.consume();
    }
}

void PatternPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    bool closePage = false;

    if (key.isPattern()) {
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
        default:
            break;
        }
        event.consume();
    }

    if (_project.playState().snapshotActive() && key.isStep()) {
        _snapshotTargetPattern = -1;
        event.consume();
    }

    bool canClose = _modal && !_latching && !_syncing && !globalKeyState()[Key::Pattern];
    if (canClose && closePage) {
        close();
    }
}

void PatternPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &playState = _project.playState();

    if (key.isContextMenu() && !_modal) {
        contextShow();
        event.consume();
        return;
    }

    if (key.pageModifier()) {
        return;
    }

    if (key.isTrackSelect()) {
        event.consume();
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::SnapRevert:
            if (playState.snapshotActive()) {
                playState.revertSnapshot(_snapshotTargetPattern);
            } else {
                playState.createSnapshot();
                _snapshotTargetPattern = -1;
            }
            break;
        case Function::SnapCommit:
            playState.commitSnapshot(_snapshotTargetPattern);
            break;
        case Function::Cancel:
            playState.cancelPatternRequests();
            break;
        default:
            break;
        }
        event.consume();
    }

    if (key.isStep()) {
        int pattern = key.step();

        if (key.shiftModifier()) {
            // select edit pattern
            _project.setSelectedPatternIndex(pattern);
        } else {
            // select playing pattern

            // use immediate by default
            // use latched when LATCH is pressed
            // use synced when SYNC is pressed or project set to always sync
            PlayState::ExecuteType executeType;
            if (_latching) executeType = PlayState::Latched;
            else if (_syncing || (_project.alwaysSyncPatterns() && _engine.state().running())) executeType = PlayState::Synced;
            else executeType = PlayState::Immediate;

            bool globalChange = true;
            for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
                if (pageKeyState()[MatrixMap::fromTrack(trackIndex)]) {
                    playState.selectTrackPattern(trackIndex, pattern, executeType);
                    globalChange = false;
                }
            }
            if (globalChange) {
                playState.selectPattern(pattern, executeType);
                _project.setSelectedPatternIndex(pattern);
            }
        }
        event.consume();
    }

    if (key.isLeft()) {
        _project.editSelectedPatternIndex(-1, false);
        event.consume();
    }
    if (key.isRight()) {
        _project.editSelectedPatternIndex(1, false);
        event.consume();
    }
}

void PatternPage::encoder(EncoderEvent &event) {
    _project.editSelectedPatternIndex(event.value(), event.pressed());
}

void PatternPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void PatternPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initPattern();
        break;
    case ContextAction::Copy:
        copyPattern();
        break;
    case ContextAction::Paste:
        pastePattern();
        break;
    case ContextAction::Duplicate:
        duplicatePattern();
        break;
    case ContextAction::Save:
        sendMidiProgramSave();
        break;
    case ContextAction::Last:
        break;
    }
}

bool PatternPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPastePattern();
    case ContextAction::Save:
        return _engine.midiProgramChangesEnabled() && _project.midiIntegrationMalekkoEnabled();
    default:
        return true;
    }
}

void PatternPage::initPattern() {
    _project.clearPattern(_project.selectedPatternIndex());
    showMessage("PATTERN INITIALIZED");
}

void PatternPage::copyPattern() {
    _model.clipBoard().copyPattern(_project.selectedPatternIndex());
    showMessage("PATTERN COPIED");
}

void PatternPage::pastePattern() {
    _model.clipBoard().pastePattern(_project.selectedPatternIndex());
    showMessage("PATTERN PASTED");
}

void PatternPage::duplicatePattern() {
    if (_project.selectedPatternIndex() < CONFIG_PATTERN_COUNT - 1) {
        _model.clipBoard().copyPattern(_project.selectedPatternIndex());
        _project.editSelectedPatternIndex(1, false);
        _model.clipBoard().pastePattern(_project.selectedPatternIndex());
        _model.clipBoard().clear();
        showMessage("PATTERN DUPLICATED");
    }
}

void PatternPage::sendMidiProgramSave() {
    if (_engine.midiProgramChangesEnabled()) {
        _engine.sendMidiProgramSave(_project.playState().trackState(0).pattern());
        showMessage("SENT MIDI PROGRAM SAVE");
    }
}
