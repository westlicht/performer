#include "TrackPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

TrackPage::TrackPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void TrackPage::enter() {
}

void TrackPage::exit() {
}

void TrackPage::draw(Canvas &canvas) {

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0);
    canvas.fill();

    canvas.setFont(Font::Tiny);
    canvas.setColor(0xf);
    canvas.drawText(2, 8 - 2, FixedStringBuilder<16>("BPM:%.1f", _engine.bpm()));
    canvas.drawText(64, 8 - 2, FixedStringBuilder<16>("TRACK%d", _project.selectedTrackIndex() + 1));

    const char *editModeLabels[] = { "GATE", "LENGTH", "NOTE" };
    canvas.drawText(128, 8 - 2, editModeLabels[int(_editMode)]);

    const char *primaryLabels[] = { "GATE", "LENGTH", "NOTE", "F4", "F5" };
    const char *secondaryLabels[] = { "SF1", "SF2", "SF3", "SF4", "SF4" };
    for (int i = 0; i < 5; ++i) {
        WindowPainter::drawFunctionKey(canvas, i, _context.keyState[Key::Shift] ? secondaryLabels[i] : primaryLabels[i], _context.keyState[MatrixMap::fromFunction(i)]);
    }

    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = track.sequence();

    canvas.setBlendMode(BlendMode::Set);

    for (int step = 0; step < 16; ++step) {

        int x = step * 16;
        int y = 20;

        canvas.setColor(step == track.currentStep() ? 0xf : 0x7);
        canvas.drawRect(x + 2, y + 2, 16 - 4, 16 - 4);

        if (_selectedSteps[step]) {
            canvas.hline(x + 2, y + 16, 16 - 4);
        }

        if (_editMode == EditMode::Note) {
            FixedStringBuilder<8> note("%d", sequence.step(step).note());
            canvas.drawText(x + 8 - canvas.textWidth(note) / 2, y + 24, note);
        }

        if (sequence.step(step).active()) {
            canvas.setColor(0xf);
            canvas.fillRect(x + 4, y + 4, 16 - 8, 16 - 8);
        }
    }

}

void TrackPage::updateLeds(Leds &leds) {
    LedPainter::drawTracksGateAndSelected(leds, _engine, _project.selectedTrackIndex());

    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = track.sequence();
    LedPainter::drawSequenceGateAndCurrentStep(leds, sequence, track.currentStep());
}

void TrackPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    // if (key.isGlobal()) {
    //     return;
    // }

    if (key.shiftModifier() && key.isTrack()) {
        return;
    }

    if (key.isTrack()) {
        _project.setSelectedTrackIndex(key.track());
        event.consume();
    }

    if (key.isStep()) {
        _selectedSteps[key.step()] = true;
        if (_editMode == EditMode::Gate) {
            _project.selectedSequence().step(key.step()).toggle();
        }
        event.consume();
    }

    if (key.isFunction()) {
        switch (key.function()) {
        case 0: _editMode = EditMode::Gate; break;
        case 1: _editMode = EditMode::Length; break;
        case 2: _editMode = EditMode::Note; break;
        default: break;
        }
        event.consume();
    }

    // if (key.isTrack()) {
    //     _engine.track(key.track()).toggleMuted();
    // }

}

void TrackPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    // if (key.isGlobal()) {
    //     return;
    // }

    if (key.isStep()) {
        _selectedSteps[key.step()] = false;
    }

    if (key.isFunction()) {
        event.consume();
    }

    // if (key.is(Key::Mute)) {
    //     close();
    // }

    // event.consume();
}

void TrackPage::encoder(EncoderEvent &event) {
    switch (_editMode) {
    case EditMode::Gate:
        break;
    case EditMode::Length:
        break;
    case EditMode::Note: {
        for (int i = 0; i < CONFIG_STEP_COUNT; ++i) {
            if (_selectedSteps[i]) {
                auto &step = _project.selectedSequence().step(i);
                step.setNote(step.note() + event.value());
            }
        }
        break;
    }
    }

    event.consume();
}
