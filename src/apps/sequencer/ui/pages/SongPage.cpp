#include "SongPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"
#include "ui/painters/SongPainter.h"

#include "model/PlayState.h"

#include "core/utils/StringBuilder.h"

enum class ContextAction {
    Init,
    Last
};

static const ContextMenuModel::Item contextMenuItems[] = {
    { "INIT" },
};

enum class Function {
    Chain       = 0,
    Add         = 1,
    Remove      = 2,
    Duplicate   = 3,
    PlayStop    = 4,
};

SongPage::SongPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void SongPage::reset() {
    setSelectedSlot(0);
}

void SongPage::enter() {
    _mode = Mode::Idle;
    setSelectedSlot(_selectedSlot);
}

void SongPage::exit() {
}

void SongPage::draw(Canvas &canvas) {
    const auto &song = _project.song();
    const auto &playState = _project.playState();
    const auto &songState = playState.songState();

    bool isShift = globalKeyState()[Key::Shift];
    bool isPlaying = songState.playing();
    const char *functionNames[] = { "CHAIN", isShift ? "INSERT" : "ADD", "REMOVE", "DUPL", isPlaying ? "STOP" : "PLAY" };

    uint8_t selectedTracks = pressedTrackKeys();

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SONG");
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    const int colWidth[] = { 16, 16, 20, 20, 20, 20, 20, 20, 20, 20 };
    const char *colHeader[] = { "#", "N", "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8" };
    const int rowHeight = 8;
    const int tableOriginX = 60;
    const int tableOriginY = 11;

    int y = tableOriginY;

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);

    auto isHighlighted = [isShift, selectedTracks] (int colIndex) {
        return
            (colIndex == 0) ||                          // always highlight id column
            (!isShift && selectedTracks == 0) ||        // highlight by default
            (isShift && colIndex == 1) ||               // highlight repeats column
            (selectedTracks & (1 << (colIndex - 2)));   // highlight track columns
    };

    // draw table header
    {
        int x = tableOriginX;
        for (int colIndex = 0; colIndex < 10; ++colIndex) {
            canvas.setColor(isHighlighted(colIndex) ? Color::Bright : Color::Medium);
            canvas.drawTextCentered(x, y, colWidth[colIndex], rowHeight, colHeader[colIndex]);
            x += colWidth[colIndex];
        }
        y += rowHeight;
    }

    // draw table entries
    for (int rowIndex = 0; rowIndex < RowCount; ++rowIndex) {
        int slotIndex = _displayRow + rowIndex;
        const auto &slot = song.slot(slotIndex);
        bool slotActive = slotIndex < song.slotCount();

        int x = tableOriginX;

        // draw play cursor
        if (songState.playing() && slotIndex == songState.currentSlot()) {
            canvas.setColor(Color::Bright);
            SongPainter::drawArrowRight(canvas, x - 4, y, 4, rowHeight);
        }

        // draw table cells
        for (int colIndex = 0; colIndex < 10; ++colIndex) {
            canvas.setColor(slotIndex == _selectedSlot && isHighlighted(colIndex) ? Color::Bright : Color::Medium);
            FixedStringBuilder<8> str;
            if (colIndex == 0) {
                str("%d", slotIndex + 1);
            } else if (colIndex == 1) {
                if (slotActive) {
                    str("%d", slot.repeats());
                } else {
                    str("-");
                }
            } else {
                if (slotActive) {
                    int trackIndex = colIndex - 2;
                    if (slot.mute(trackIndex)) {
                        str("M");
                    } else {
                        str("P%d", slot.pattern(trackIndex) + 1);
                    }
                } else {
                    str("-");
                }
            }
            canvas.drawTextCentered(x, y, colWidth[colIndex], rowHeight, str);
            x += colWidth[colIndex];
        }
        y += rowHeight;
    }

    WindowPainter::drawScrollbar(canvas, 252, tableOriginY + rowHeight + 2, 4, RowCount * rowHeight - 2, song.slotCount(), RowCount, _displayRow);

    // draw play info
    if (songState.playing()) {
        int currentSlot = songState.currentSlot();
        int currentRepeat = songState.currentRepeat();
        int repeats = song.slot(currentSlot).repeats();
        float slotProgress = (currentRepeat + _engine.measureFraction()) / repeats;

        uint32_t beatsPerMeasure = _project.timeSignature().beats();
        uint32_t beat = _engine.tick() / _engine.noteDivisor();

        canvas.setBlendMode(BlendMode::Set);
        canvas.setColor(Color::Bright);

        canvas.setFont(Font::Tiny);
        canvas.drawTextCentered(8, 10, 32, 10, FixedStringBuilder<16>("%d.%d", beat / beatsPerMeasure + 1, beat % beatsPerMeasure + 1));

        canvas.setFont(Font::Small);
        canvas.drawTextCentered(8, 25, 32, 10, FixedStringBuilder<8>("S%d", currentSlot + 1));

        canvas.setFont(Font::Tiny);
        SongPainter::drawProgress(canvas, 8, 40, 32, 2, slotProgress);
    }

    if (playState.hasSyncedRequests() && songState.hasPlayRequests()) {
        canvas.setColor(Color::Bright);
        canvas.hline(0, 10, _engine.syncFraction() * Width);
    }
}

void SongPage::updateLeds(Leds &leds) {
    bool isShift = globalKeyState()[Key::Shift];
    uint8_t selectedTracks = pressedTrackKeys();

    LedPainter::drawTrackGates(leds, _engine, _project.playState());

    if (_selectedSlot >= 0) {
        const auto &slot = _project.song().slot(_selectedSlot);
        if (isShift) {
            int repeats = slot.repeats();
            if (repeats <= 16) {
                leds.set(MatrixMap::fromStep(repeats - 1), true, true);
            }
        } else {
            uint16_t usedPatterns = 0;
            for (int trackIndex = 0; trackIndex < 8; ++trackIndex) {
                if (selectedTracks == 0 || selectedTracks & (1 << trackIndex)) {
                    usedPatterns |= (1 << slot.pattern(trackIndex));
                }
            }
            LedPainter::drawSongSlot(leds, usedPatterns);
        }
    }
}

void SongPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Chain:
            _mode = Mode::Chain;
            break;
        default:
            break;
        }
        event.consume();
    }
}

void SongPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isFunction()) {
        _mode = Mode::Idle;
        event.consume();
    }
}

void SongPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &song = _project.song();
    auto &playState = _project.playState();
    uint8_t selectedTracks = pressedTrackKeys();

    if (key.isContextMenu()) {
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
        case Function::Add:
            if (key.shiftModifier()) {
                song.insertSlot(std::max(0, _selectedSlot));
                setSelectedSlot(_selectedSlot);
            } else {
                song.insertSlot(song.slotCount());
                setSelectedSlot(song.slotCount());
            }
            break;
        case Function::Remove:
            song.removeSlot(_selectedSlot);
            setSelectedSlot(_selectedSlot);
            break;
        case Function::Duplicate:
            song.duplicateSlot(_selectedSlot);
            setSelectedSlot(_selectedSlot + 1);
            break;
        case Function::PlayStop:
            if (playState.songState().playing()) {
                playState.stopSong();
            } else {
                playState.playSong(_selectedSlot, (key.shiftModifier() && _engine.clockRunning()) ? PlayState::ExecuteType::Synced : PlayState::ExecuteType::Immediate);
            }
            break;
        default:
            break;
        }

        event.consume();
    }

    if (key.isEncoder()) {
        if (selectedTracks) {
            for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
                if (selectedTracks & (1 << trackIndex)) {
                    _project.song().toggleMute(_selectedSlot, trackIndex);
                }
            }
        } else {
            playState.playSong(_selectedSlot, (key.shiftModifier() && _engine.clockRunning()) ? PlayState::ExecuteType::Synced : PlayState::ExecuteType::Immediate);
        }

        event.consume();
    }

    if (key.isStep()) {
        if (key.shiftModifier()) {
            song.setRepeats(_selectedSlot, key.step() + 1);
        } else {
            int pattern = key.step();

            switch (_mode) {
            case Mode::Idle: {
                bool globalChange = true;
                for (int trackIndex = 0; trackIndex < 8; ++trackIndex) {
                    if (globalKeyState()[MatrixMap::fromTrack(trackIndex)]) {
                        song.setPattern(_selectedSlot, trackIndex, pattern);
                        globalChange = false;
                    }
                }
                if (globalChange) {
                    song.setPattern(_selectedSlot, pattern);
                }
                break;
            }
            case Mode::Chain:
                song.chainPattern(pattern);
                if (!playState.songState().playing()) {
                    playState.playSong(0);
                }
                setSelectedSlot(SlotCount);
                break;
            default:
                break;
            }
        }

        event.consume();
    }

    if (key.isLeft()) {
        moveSelectedSlot(-1, key.shiftModifier());
        event.consume();
    }
    if (key.isRight()) {
        moveSelectedSlot(1, key.shiftModifier());
        event.consume();
    }
}

void SongPage::encoder(EncoderEvent &event) {
    bool isShift = globalKeyState()[Key::Shift];
    uint8_t selectedTracks = pressedTrackKeys();

    if (isShift) {
        _project.song().editRepeats(_selectedSlot, event.value());
    } else if (selectedTracks) {
        for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
            if (selectedTracks & (1 << trackIndex)) {
                _project.song().editPattern(_selectedSlot, trackIndex, event.value());
            }
        }
    } else {
        moveSelectedSlot(event.value(), false);
    }

    event.consume();
}

void SongPage::setSelectedSlot(int slot) {
    int slotCount = _project.song().slotCount();
    _selectedSlot = slotCount > 0 ? clamp(slot, 0, slotCount - 1) : -1;
    scrollTo(_selectedSlot);
}

void SongPage::moveSelectedSlot(int offset, bool moveSlot) {
    if (moveSlot) {
        _project.song().swapSlot(_selectedSlot, _selectedSlot + offset);
    }
    setSelectedSlot(_selectedSlot + offset);
}

void SongPage::scrollTo(int row) {
    if (row < _displayRow) {
        _displayRow = std::max(0, row);
    } else if (row >= _displayRow + RowCount) {
        _displayRow = row - (RowCount - 1);
    }
    if (_displayRow + RowCount >= _project.song().slotCount()) {
        _displayRow = std::max(0, _project.song().slotCount() - RowCount);
    }
}

uint8_t SongPage::pressedTrackKeys() const {
    uint8_t tracks = 0;
    for (int trackIndex = 0; trackIndex < CONFIG_TRACK_COUNT; ++trackIndex) {
        if (globalKeyState()[MatrixMap::fromTrack(trackIndex)]) {
            tracks |= (1 << trackIndex);
        }
    }
    return tracks;
}

void SongPage::contextShow() {
    showContextMenu(ContextMenu(
        contextMenuItems,
        int(ContextAction::Last),
        [&] (int index) { contextAction(index); },
        [&] (int index) { return contextActionEnabled(index); }
    ));
}

void SongPage::contextAction(int index) {
    switch (ContextAction(index)) {
    case ContextAction::Init:
        initSong();
        break;
    case ContextAction::Last:
        break;
    }
}

bool SongPage::contextActionEnabled(int index) const {
    switch (ContextAction(index)) {
    default:
        return true;
    }
}

void SongPage::initSong() {
    _project.playState().stopSong();
    _project.song().clear();
    setSelectedSlot(_selectedSlot);
    showMessage("SONG INITIALIZED");
}
