#include "NoteSequencePage.h"

#include "ui/LedPainter.h"
#include "ui/painters/SequencePainter.h"
#include "ui/painters/WindowPainter.h"

#include "engine/Scale.h"

#include "os/os.h"

#include "core/utils/StringBuilder.h"

enum class Function {
    Gate        = 0,
    Length      = 1,
    Index       = 2,
    Condition   = 3,
};

static const char *functionNames[] = { "GATE", "LENGTH", "INDEX", "COND", nullptr };


enum class MenuItem {
    Clear,
    Copy,
    Paste,
    Last
};

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
    WindowPainter::drawFunctionKeys(canvas, functionNames, _keyState);

    const auto &sequenceEngine = _engine.selectedTrackEngine().noteSequenceEngine();
    const auto &sequence = _project.selectedSequence().noteSequence();
    int currentStep = sequenceEngine.isActiveSequence(sequence) ? sequenceEngine.currentStep() : -1;

    const auto &scale = Scale::get(sequence.scale());

    bool drawStepIndices = true;
    bool drawSelectedSteps = true;
    bool drawGateProbabilities = _mode == Mode::GateVariation;
    bool drawLengths = _mode == Mode::Length;
    bool drawLengthVariations = _mode == Mode::LengthVariation;
    bool drawNotes = _mode == Mode::Note;
    // bool drawLengthProbabilities = _mode == Mode::LengthVariation;

    canvas.setBlendMode(BlendMode::Set);

    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset() + i;
        const auto &step = sequence.step(stepIndex);

        int x = i * 16;
        int y = 20;

        if (drawStepIndices) {
            canvas.setColor(0x7);
            if (drawSelectedSteps && _stepSelection[stepIndex]) {
                canvas.setColor(0xf);
            }
            FixedStringBuilder<8> str("%d", stepIndex + 1);
            canvas.drawText(x + (16 - canvas.textWidth(str)) / 2, y - 2, str);
        }

        canvas.setColor(stepIndex == currentStep ? 0xf : 0x7);
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

    if (_mode != Mode::Gate && os::ticks() < _lastEditTicks + os::time::ms(500)) {
        drawDetail(canvas, sequence.step(_stepSelection.first()));
    }
}

void NoteSequencePage::updateLeds(Leds &leds) {
    const auto &sequenceEngine = _engine.selectedTrackEngine().noteSequenceEngine();
    const auto &sequence = _project.selectedSequence().noteSequence();
    int currentStep = sequenceEngine.isActiveSequence(sequence) ? sequenceEngine.currentStep() : -1;

    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset() + i;
        bool red = (stepIndex == currentStep) || _stepSelection[stepIndex];
        bool green = (stepIndex != currentStep) && (sequence.step(stepIndex).gate() || _stepSelection[stepIndex]);
        leds.set(MatrixMap::fromStep(i), red, green);
    }

    LedPainter::drawSelectedSequencePage(leds, _page);
}

void NoteSequencePage::keyDown(KeyEvent &event) {
    _stepSelection.keyDown(event, stepOffset());
}

void NoteSequencePage::keyUp(KeyEvent &event) {
    _stepSelection.keyUp(event, stepOffset());
}

void NoteSequencePage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();
    auto &sequence = _project.selectedSequence().noteSequence();

    _stepSelection.keyPress(event, stepOffset());

    if (!key.shiftModifier() && key.isStep()) {
        int stepIndex = stepOffset() + key.step();
        switch (_mode) {
        case Mode::Gate:
            sequence.step(stepIndex).toggleGate();
            event.consume();
            break;
        default:
            break;
        }
    }

    if (key.isFunction()) {
        switch (Function(key.function())) {
        case Function::Gate:
            _mode = _mode == Mode::Gate ? Mode::GateVariation : Mode::Gate;
            break;
        case Function::Length:
            _mode = _mode == Mode::Length ? Mode::LengthVariation : Mode::Length;
            break;
        case Function::Index:
            _mode = _mode == Mode::Note ? Mode::NoteVariation : Mode::Note;
            break;
        case Function::Condition:
            _mode = Mode::TrigCondition;
            break;
        }
        event.consume();
    }

    if (key.is(Key::Left)) {
        if (key.shiftModifier()) {
            sequence.shift(-1);
        } else {
            _page = std::max(0, _page - 1);
        }
        event.consume();
    }
    if (key.is(Key::Right)) {
        if (key.shiftModifier()) {
            sequence.shift(1);
        } else {
            _page = std::min(3, _page + 1);
        }
        event.consume();
    }
}

void NoteSequencePage::encoder(EncoderEvent &event) {
    auto &sequence = _project.selectedSequence().noteSequence();
    const auto &scale = Scale::get(sequence.scale());

    if (_stepSelection.any()) {
        _lastEditTicks = os::ticks();
    }

    for (size_t i = 0; i < sequence.steps().size(); ++i) {
        if (_stepSelection[i]) {
            auto &step = sequence.step(i);
            switch (_mode) {
            case Mode::Gate:
                step.setGate(event.value() > 0);
                break;
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

    const auto &scale = Scale::get(_project.selectedSequence().noteSequence().scale());

    FixedStringBuilder<16> str;

    WindowPainter::drawFrame(canvas, 64, 16, 128, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.vline(64 + 32, 16, 32);

    canvas.setFont(Font::Small);
    str("%d", _stepSelection.first() + 1);
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
