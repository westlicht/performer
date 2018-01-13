#include "SequenceSetupPage.h"

#include "ui/LedPainter.h"
#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

#if 0
static const char *ParameterNameShort[] = {
    "DIV",
    "MODE",
    "FIRST",
    "LAST",
    "RST",
    "SCALE",
    "SHIFT",
    "",
    "",
    "",
};

static const char *ParameterNameLong[] = {
    "Divider",
    "Play Mode",
    "First Step",
    "Last Step",
    "Reset Measure",
    "Scale",
    "Shift",
    "",
    "",
    "",
};
#endif

static const char *playModeToStr(Sequence::PlayMode playMode) {
    switch (playMode) {
    case Sequence::Forward:     return "Forward";
    case Sequence::Backward:    return "Backward";
    case Sequence::PingPong:    return "Ping Pong";
    case Sequence::Pendulum:    return "Pendulum";
    case Sequence::Random:      return "Random";
    default:                    return "INVALID";
    };
}

struct ParameterHandler {
    const char *shortName;
    const char *longName;
    std::function<void(const Sequence &, StringBuilder &)> formatter;
    std::function<void(Sequence &, EncoderEvent &)> encoder;
};

static const ParameterHandler ParameterHandlers[] = {
    {
        "DIV", "Divider",
        [] (const Sequence &sequence, StringBuilder &str) {},
        [] (Sequence &sequence, EncoderEvent &event) {},
    },
    {
        "MODE", "Play Mode",
        [] (const Sequence &sequence, StringBuilder &str) {
            str("%s", playModeToStr(sequence.playMode()));
        },
        [] (Sequence &sequence, EncoderEvent &event) {
            sequence.setPlayMode(Sequence::PlayMode(int(sequence.playMode()) + event.value()));
        },
    },
    {
        "FIRST", "First Step",
        [] (const Sequence &sequence, StringBuilder &str) {
            str("%d", sequence.firstStep() + 1);
        },
        [] (Sequence &sequence, EncoderEvent &event) {
            sequence.setFirstStep(sequence.firstStep() + event.value());
        },
    },
    {
        "LAST", "Last Step",
        [] (const Sequence &sequence, StringBuilder &str) {
            str("%d", sequence.lastStep() + 1);
        },
        [] (Sequence &sequence, EncoderEvent &event) {
            sequence.setLastStep(sequence.lastStep() + event.value());
        },
    },
    {
        "RESET", "Reset Measure",
        [] (const Sequence &sequence, StringBuilder &str) {},
        [] (Sequence &sequence, EncoderEvent &event) {},
    },
    {
        "SCALE", "Scale",
        [] (const Sequence &sequence, StringBuilder &str) {},
        [] (Sequence &sequence, EncoderEvent &event) {},
    },
    {
        "SHIFT", "Shift",
        [] (const Sequence &sequence, StringBuilder &str) {},
        [] (Sequence &sequence, EncoderEvent &event) {},
    },
    {
        "TRANS", "Transpose",
        [] (const Sequence &sequence, StringBuilder &str) {},
        [] (Sequence &sequence, EncoderEvent &event) {},
    },
    {
        "", "",
        [] (const Sequence &sequence, StringBuilder &str) {},
        [] (Sequence &sequence, EncoderEvent &event) {},
    },
    {
        "", "",
        [] (const Sequence &sequence, StringBuilder &str) {},
        [] (Sequence &sequence, EncoderEvent &event) {},
    },

};

SequenceSetupPage::SequenceSetupPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void SequenceSetupPage::enter() {
}

void SequenceSetupPage::exit() {
}

void SequenceSetupPage::draw(Canvas &canvas) {

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0);
    canvas.fill();

    canvas.setFont(Font::Tiny);
    canvas.setColor(0xf);
    canvas.drawText(2, 8 - 2, FixedStringBuilder<16>("BPM:%.1f", _engine.bpm()));
    canvas.drawText(64, 8 - 2, FixedStringBuilder<16>("TRACK%d", _project.selectedTrackIndex() + 1));

    canvas.drawText(128, 8 - 2, ParameterHandlers[_parameter].shortName);

    for (int i = 0; i < 5; ++i) {
        WindowPainter::drawFunctionKey(
            canvas, i,
            _context.keyState[Key::Shift] ? ParameterHandlers[i + 5].shortName : ParameterHandlers[i].shortName,
            _context.keyState[MatrixMap::fromFunction(i)]
        );
    }

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);
    canvas.setFont(Font::Small);
    canvas.drawText(8, 32, ParameterHandlers[_parameter].longName);

    FixedStringBuilder<32> sb;
    ParameterHandlers[_parameter].formatter(_project.selectedSequence(), sb);
    canvas.drawText(128, 32, sb);

}

void SequenceSetupPage::updateLeds(Leds &leds) {
    LedPainter::drawTracksGateAndSelected(leds, _engine, _project.selectedTrackIndex());

    const auto &track = _engine.track(_project.selectedTrackIndex());
    const auto &sequence = track.sequence();
    LedPainter::drawSequenceGateAndCurrentStep(leds, sequence, track.currentStep());
    // const auto &track = _engine.track(_project.selectedTrackIndex());
    // const auto &sequence = track.sequence();
    // LedPainter::drawSequenceGateAndCurrentStep(leds, sequence, track.currentStep());
}

void SequenceSetupPage::keyDown(KeyEvent &event) {
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

    // if (key.isStep()) {
    //     _selectedSteps[key.step()] = true;
    //     if (_editMode == EditMode::Gate) {
    //         _project.selectedSequence().step(key.step()).toggle();
    //     }
    //     event.consume();
    // }

    if (key.isFunction()) {
        _parameter = key.function() + (key.shiftModifier() ? 5 : 0);
        event.consume();
    }

    // if (key.isTrack()) {
    //     _engine.track(key.track()).toggleMuted();
    // }

}

void SequenceSetupPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    // if (key.isGlobal()) {
    //     return;
    // }

    // if (key.isStep()) {
    //     _selectedSteps[key.step()] = false;
    // }

    if (key.isFunction()) {
        event.consume();
    }

    // if (key.is(Key::Mute)) {
    //     close();
    // }

    // event.consume();
}

void SequenceSetupPage::encoder(EncoderEvent &event) {
    ParameterHandlers[_parameter].encoder(_project.selectedSequence(), event);
    event.consume();
}

