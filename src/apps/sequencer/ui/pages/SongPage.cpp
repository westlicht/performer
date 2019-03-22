#include "SongPage.h"

#include "Pages.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"
#include "ui/painters/SongPainter.h"

#include "model/PlayState.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Clear       = 0,
    Chain       = 1,
    Add         = 2,
    Remove      = 3,
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

    bool isPlaying = songState.playing();
    const char *functionNames[] = { "CLEAR", "CHAIN", "ADD", "REMOVE", isPlaying ? "STOP" : "PLAY" };

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SONG");
    WindowPainter::drawFooter(canvas, functionNames, pageKeyState());

    const int slotWidth = Width / SlotCount;

    float syncMeasureFraction = _engine.syncMeasureFraction();

    // draw selection cursor
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    SongPainter::drawArrowDown(canvas, _selectedSlot * slotWidth + 1, 16, slotWidth - 2);

    // draw play cursor and slot progress
    if (songState.playing()) {
        SongPainter::drawArrowUp(canvas, songState.currentSlot() * slotWidth + 1, 44, slotWidth - 2);

        int currentSlot = songState.currentSlot();
        float progress = (songState.currentRepeat() + syncMeasureFraction) / song.slot(currentSlot).repeats();
        SongPainter::drawProgress(canvas, currentSlot * slotWidth + 2, 48, slotWidth - 4, 2, progress);
    }

    for (int i = 0; i < SlotCount; ++i) {
        int slotIndex = i;
        const auto &slot = song.slot(slotIndex);
        bool active = slotIndex < song.slotCount();

        int x = i * slotWidth;
        int y = 20;

        // pattern block
        canvas.setColor(active ? 0xf : 0x7);
        canvas.drawRect(x + 2, y + 2, slotWidth - 4, slotWidth - 4);

        // details
        if (active) {
            canvas.setColor(0xf);
            FixedStringBuilder<8> patternStr("%d", slot.pattern(_project.selectedTrackIndex()) + 1);
            canvas.drawText(x + (slotWidth - canvas.textWidth(patternStr) + 1) / 2, y + 9, patternStr);
            if (slot.repeats() > 0) {
                FixedStringBuilder<8> repeatsStr("*%d", slot.repeats());
                canvas.drawText(x + (slotWidth - canvas.textWidth(repeatsStr) + 1) / 2, y + 20, repeatsStr);
            }
        }
    }

    if (playState.hasSyncedRequests() && songState.hasPlayRequests()) {
        canvas.setColor(0xf);
        canvas.hline(0, 10, syncMeasureFraction * Width);
    }
}

void SongPage::updateLeds(Leds &leds) {
    if (_selectedSlot >= 0) {
        const auto &slot = _project.song().slot(_selectedSlot);
        uint16_t usedPatterns = 0;
        for (int trackIndex = 0; trackIndex < 8; ++trackIndex) {
            usedPatterns |= (1<<slot.pattern(trackIndex));
        }
        LedPainter::drawSongSlot(leds, slot.pattern(_project.selectedTrackIndex()), usedPatterns);
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

    if (key.pageModifier()) {
        return;
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Clear:
            playState.stopSong();
            song.clear();
            setSelectedSlot(_selectedSlot);
            break;
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
        case Function::PlayStop:
            if (playState.songState().playing()) {
                playState.stopSong();
            } else {
                playState.playSong(_selectedSlot, key.shiftModifier() ? PlayState::ExecuteType::Synced : PlayState::ExecuteType::Immediate);
                if (!_engine.clockRunning()) {
                    _engine.clockStart();
                }
            }
            break;
        default:
            break;
        }

        event.consume();
    }

    if (key.isStep()) {
        int pattern = key.step();

        switch (_mode) {
        case Mode::Idle:
            if (_selectedSlot >= 0) {
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
            }
            break;
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
    if (event.pressed()) {
        _project.song().editRepeats(_selectedSlot, event.value());
    } else {
        moveSelectedSlot(event.value(), globalKeyState()[Key::Shift]);
    }
}

void SongPage::setSelectedSlot(int slot) {
    int slotCount = _project.song().slotCount();
    _selectedSlot = slotCount > 0 ? clamp(slot, 0, slotCount - 1) : -1;
}

void SongPage::moveSelectedSlot(int offset, bool moveSlot) {
    if (moveSlot) {
        _project.song().swapSlot(_selectedSlot, _selectedSlot + offset);
    }
    setSelectedSlot(_selectedSlot + offset);
}
