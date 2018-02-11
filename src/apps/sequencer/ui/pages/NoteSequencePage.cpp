#include "NoteSequencePage.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "engine/Scale.h"

#include "core/utils/StringBuilder.h"

NoteSequencePage::NoteSequencePage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void NoteSequencePage::enter() {
}

void NoteSequencePage::exit() {
}

void NoteSequencePage::draw(Canvas &canvas) {

    WindowPainter::clear(canvas);
    WindowPainter::drawHeader(canvas, _model, _engine, "SEQUENCE");
    WindowPainter::drawActiveFunction(canvas, modeName(_mode));

    const char *functionNames[] = { "GATE", "LENGTH", "INDEX", nullptr, nullptr };
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = _project.selectedSequence();
    const auto &noteSequence = sequence.noteSequence();
    const auto &scale = Scale::scale(noteSequence.scale());

    bool drawStepIndices = true;
    bool drawSelectedSteps = _mode != Mode::Gate;
    bool drawGateProbabilities = _mode == Mode::GateVariation;
    bool drawLengths = _mode == Mode::Length;
    bool drawLengthVariations = _mode == Mode::LengthVariation;
    bool drawNotes = _mode == Mode::Note;
    // bool drawLengthProbabilities = _mode == Mode::LengthVariation;

    canvas.setBlendMode(BlendMode::Set);

    for (int stepIndex = 0; stepIndex < 16; ++stepIndex) {
        const auto &step = noteSequence.step(stepIndex);

        int x = stepIndex * 16;
        int y = 20;

        if (drawStepIndices) {
            canvas.setColor(0x7);
            if (drawSelectedSteps && (_selectedSteps[stepIndex] || _keyState[Key::Shift])) {
                canvas.setColor(0xf);
            }
            FixedStringBuilder<8> str("%d", stepIndex + 1);
            canvas.drawText(x + (16 - canvas.textWidth(str)) / 2, y - 2, str);
        }

        canvas.setColor(stepIndex == track.currentStep() ? 0xf : 0x7);
        canvas.drawRect(x + 2, y + 2, 16 - 4, 16 - 4);
        if (step.gate()) {
            canvas.setColor(0xf);
            canvas.fillRect(x + 4, y + 4, 16 - 8, 16 - 8);
        }

        if (drawGateProbabilities) {
            SequencePainter::drawProbability(
                canvas,
                x + 2, y + 18, 16 - 4, 2,
                step.gateProbability() + 1, NoteSequence::GateProbability::Range
            );
        }

        if (drawLengths) {
            SequencePainter::drawLength(
                canvas,
                x + 2, y + 18, 16 - 4, 6,
                step.length() + 1, NoteSequence::Length::Range
            );
        }

        if (drawLengthVariations) {
            SequencePainter::drawLengthRange(
                canvas,
                x + 2, y + 18, 16 - 4, 6,
                step.length() + 1, step.lengthVariationRange(), NoteSequence::Length::Range
            );
            SequencePainter::drawProbability(
                canvas,
                x + 2, y + 28, 16 - 4, 2,
                step.lengthVariationProbability(), NoteSequence::LengthVariationProbability::Max
            );
        }

        if (drawNotes) {
            canvas.setColor(0xf);
            FixedStringBuilder<8> str;
            scale.shortName(step.note(), 0, str);
            canvas.drawText(x + 2, y + 20, str);
            str.reset();
            scale.shortName(step.note(), 1, str);
            canvas.drawText(x + 2, y + 27, str);
        }

    }

    if (_mode != Mode::Gate && (_selectedSteps.any() || _keyState[Key::Shift])) {
        drawDetail(canvas, noteSequence.step(_selectedSteps.first()));
    }
}

void NoteSequencePage::updateLeds(Leds &leds) {
    // LedPainter::drawTracksGateAndSelected(leds, _engine, _project.selectedTrackIndex());

    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = track.sequence();
    LedPainter::drawSequenceGateAndCurrentStep(leds, sequence, track.currentStep());
}

void NoteSequencePage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    auto &noteSequence = _project.selectedSequence().noteSequence();

    if (key.isStep()) {
        _selectedSteps.add(key.step());
        switch (_mode) {
        case Mode::Gate:
            noteSequence.step(key.step()).toggleGate();
            break;
        default:
            break;
        }
        event.consume();
    }

    if (key.isFunction()) {
        switch (key.function()) {
        case 0: _mode = _mode == Mode::Gate ? Mode::GateVariation : Mode::Gate; break;
        case 1: _mode = _mode == Mode::Length ? Mode::LengthVariation : Mode::Length; break;
        case 2: _mode = _mode == Mode::Note ? Mode::NoteVariation : Mode::Note; break;
        default: break;
        }
        event.consume();
    }
}

void NoteSequencePage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isStep()) {
        _selectedSteps.remove(key.step());
        event.consume();
    }

    if (key.isFunction()) {
        event.consume();
    }
}

void NoteSequencePage::encoder(EncoderEvent &event) {
    auto &noteSequence = _project.selectedSequence().noteSequence();
    const auto &scale = Scale::scale(noteSequence.scale());

    for (size_t i = 0; i < noteSequence.steps().size(); ++i) {
        if (_selectedSteps[i] || _keyState[Key::Shift]) {
            auto &step = noteSequence.step(i);
            switch (_mode) {
            case Mode::GateVariation:
                step.setGateProbability(NoteSequence::GateProbability::clamp(step.gateProbability() + event.value()));
                break;
            case Mode::Length:
                step.setLength(NoteSequence::Length::clamp(step.length() + event.value()));
                break;
            case Mode::LengthVariation:
                if (event.pressed()) {
                    step.setLengthVariationProbability(NoteSequence::LengthVariationProbability::clamp(step.lengthVariationProbability() + event.value()));
                } else {
                    step.setLengthVariationRange(NoteSequence::LengthVariationRange::clamp(step.lengthVariationRange() + event.value()));
                }
                break;
            case Mode::Note:
                step.setNote(NoteSequence::Note::clamp(step.note() + event.value() * (event.pressed() ? scale.octave() : 1)));
                break;
            case Mode::NoteVariation:
                break;
            default:
                break;
            }
        }
    }

    event.consume();
}

void NoteSequencePage::drawDetail(Canvas &canvas, const NoteSequence::Step &step) {

    const auto &scale = Scale::scale(_project.selectedSequence().noteSequence().scale());

    FixedStringBuilder<16> str;

    WindowPainter::drawFrame(canvas, 64, 16, 128, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.vline(64 + 32, 16, 32);

    canvas.setFont(Font::Small);
    str("%d", _selectedSteps.first() + 1);
    canvas.drawTextCentered(64, 16, 32, 32, str);

    canvas.setFont(Font::Tiny);

    switch (_mode) {
    case Mode::GateVariation:
        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.gateProbability() + 1, NoteSequence::GateProbability::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.gateProbability() + 1.f) / NoteSequence::GateProbability::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Mode::Length:
        SequencePainter::drawLength(
            canvas,
            64 + 32 + 8, 32 - 4, 64 - 16, 8,
            step.length() + 1, NoteSequence::Length::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.length() + 1.f) / NoteSequence::Length::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 4, 32, 8, str);
        break;
    case Mode::LengthVariation:
        SequencePainter::drawLengthRange(
            canvas,
            64 + 32 + 8, 32 - 12, 64 - 16, 8,
            step.length() + 1, step.lengthVariationRange(), NoteSequence::Length::Range
        );
        str.reset();
        str("%.1f%%", 100.f * (step.lengthVariationRange()) / NoteSequence::Length::Range);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 - 12, 32, 8, str);

        SequencePainter::drawProbability(
            canvas,
            64 + 32 + 8, 32 + 4, 64 - 16, 8,
            step.lengthVariationProbability(), NoteSequence::LengthVariationProbability::Max
        );
        str.reset();
        str("%.1f%%", 100.f * (step.lengthVariationProbability()) / NoteSequence::LengthVariationProbability::Max);
        canvas.setColor(0xf);
        canvas.drawTextCentered(64 + 32 + 64, 32 + 4, 32, 8, str);
        break;
    case Mode::Note:
        str.reset();
        scale.longName(step.note(), str);
        canvas.setFont(Font::Small);
        canvas.drawTextCentered(64 + 32, 16, 64, 32, str);
        break;
    default:
        break;
    }
}
