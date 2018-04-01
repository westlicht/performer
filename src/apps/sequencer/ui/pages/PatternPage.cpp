#include "PatternPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "model/PlayState.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Edit    = 0,
};

static const char *functionNames[] = { "EDIT", nullptr, nullptr, nullptr, nullptr };

enum class ContextAction {
    Init,
    Copy,
    Paste,
    Duplicate,
    Last
};

const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
    { "COPY" },
    { "PASTE" },
    { "DUP"},
};


PatternPage::PatternPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context),
    _contextMenu(
        manager.pages().contextMenu,
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    )
{}

void PatternPage::enter() {
}

void PatternPage::exit() {
}

void PatternPage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "PATTERN");
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    constexpr int Border = 4;

    const auto &playState = _project.playState();

    float syncMeasureFraction = _engine.syncMeasureFraction();
    bool hasRequested = false;

    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);

    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        const auto &trackEngine = _engine.trackEngine(trackIndex);
        const auto &trackState = playState.trackState(trackIndex);
        bool trackSelected = _keyState[MatrixMap::fromTrack(trackIndex)];

        int x = trackIndex * 32;
        int y = 16;

        int w = 16;
        int h = 16;

        x += 8;

        canvas.setColor(trackSelected ? 0xf : 0x7);
        canvas.drawTextCentered(x, y - 2, w, 8, FixedStringBuilder<8>("T%d", trackIndex + 1));

        y += 8;

        canvas.setColor(trackEngine.activity() ? 0xf : 0x7);
        canvas.drawRect(x, y, w, h);
        if (trackState.mute()) {
            canvas.setColor(0xf);
            canvas.fillRect(x + Border, y + Border, w - 2 * Border, h - 2 * Border);
        }

        y += 8;

        canvas.setColor(trackSelected ? 0xf : 0x7);
        canvas.drawTextCentered(x, y + 10, w, 8, FixedStringBuilder<8>("P%d", trackState.pattern() + 1));

        if (trackState.pattern() != trackState.requestedPattern()) {
            hasRequested = true;
        }
    }

    if (hasRequested) {
        canvas.setColor(0xf);
        canvas.hline(0, 10, syncMeasureFraction * Width);
    }
}

void PatternPage::updateLeds(Leds &leds) {
    const auto &playState = _project.playState();

    if (_keyState[MatrixMap::fromFunction(int(Function::Edit))]) {
        LedPainter::drawSelectedPattern(leds, _project.selectedPatternIndex(), _project.selectedPatternIndex());
    } else {
        uint16_t allActivePatterns = 0;
        uint16_t allRequestedPatterns = 0;
        uint16_t selectedActivePatterns = 0;
        uint16_t selectedRequestedPatterns = 0;

        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            const auto &trackState = playState.trackState(trackIndex);
            allActivePatterns |= (trackState.pattern() < 16) ? (1<<trackState.pattern()) : 0;
            allRequestedPatterns |= (trackState.requestedPattern() < 16) ? (1<<trackState.requestedPattern()) : 0;
            if (_keyState[MatrixMap::fromTrack(trackIndex)]) {
                selectedActivePatterns |= (trackState.pattern() < 16) ? (1<<trackState.pattern()) : 0;
                selectedRequestedPatterns |= (trackState.requestedPattern() < 16) ? (1<<trackState.requestedPattern()) : 0;
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
}

void PatternPage::keyUp(KeyEvent &event) {
}

void PatternPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &playState = _project.playState();

    if (key.isContextMenu()) {
        _contextMenu.show();
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
        event.consume();
    }

    if (key.isStep()) {
        int pattern = key.step();

        if (_keyState[MatrixMap::fromFunction(int(Function::Edit))]) {
            // select edit pattern
            _project.setSelectedPatternIndex(pattern);
        } else {
            // select playing pattern
            bool globalChange = true;
            for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
                if (_keyState[MatrixMap::fromTrack(trackIndex)]) {
                    playState.selectTrackPattern(trackIndex, pattern, key.shiftModifier() ? PlayState::Scheduled : PlayState::Immediate);
                    globalChange = false;
                }
            }
            if (globalChange) {
                playState.selectPattern(pattern, key.shiftModifier() ? PlayState::Scheduled : PlayState::Immediate);
            }
        }

        event.consume();
    }

    if (key.is(Key::Left)) {
        _project.editSelectedPatternIndex(-1, false);
        event.consume();
    }
    if (key.is(Key::Right)) {
        _project.editSelectedPatternIndex(1, false);
        event.consume();
    }
}

void PatternPage::encoder(EncoderEvent &event) {
    _project.editSelectedPatternIndex(event.value(), event.pressed());
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
    case ContextAction::Last:
        break;
    }
}

bool PatternPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    case ContextAction::Paste:
        return _model.clipBoard().canPastePattern();
    default:
        return true;
    }
}

void PatternPage::initPattern() {
    _project.clearPattern(_project.selectedPatternIndex());
}

void PatternPage::copyPattern() {
    _model.clipBoard().copyPattern(_project, _project.selectedPatternIndex());
}

void PatternPage::pastePattern() {
    _model.clipBoard().pastePattern(_project, _project.selectedPatternIndex());
}

void PatternPage::duplicatePattern() {
    if (_project.selectedPatternIndex() < CONFIG_PATTERN_COUNT - 1) {
        _model.clipBoard().copyPattern(_project, _project.selectedPatternIndex());
        _project.editSelectedPatternIndex(1, false);
        _model.clipBoard().pastePattern(_project, _project.selectedPatternIndex());
        _model.clipBoard().clear();
    }
}
