#include "LedPainter.h"
#include "Leds.h"
#include "MatrixMap.h"
#include "Key.h"

#include "engine/Engine.h"

#include "model/NoteSequence.h"

// color guidelines:
// yellow -> selection
// green -> active
// red -> inactive

void LedPainter::drawTracksGateAndSelected(Leds &leds, const Engine &engine, int selectedTrack) {
    for (int track = 0; track < 8; ++track) {
        // leds.set(MatrixMap::fromTrack(track), engine.trackEngine(track).activity(), track == selectedTrack);
        const auto &trackEngine = engine.trackEngine(track);
        bool unmutedActivity = trackEngine.activity() && !trackEngine.mute();
        bool mutedActivity = trackEngine.activity() && trackEngine.mute();
        bool selected = track == selectedTrack;
        leds.set(
            MatrixMap::fromTrack(track),
            mutedActivity || (selected && !unmutedActivity),
            unmutedActivity || (selected && !mutedActivity)
        );
    }
}

void LedPainter::drawNoteSequenceGateAndCurrentStep(Leds &leds, const NoteSequence &sequence, int stepOffset, int currentStep) {
    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset + i;
        leds.set(MatrixMap::fromStep(i), stepIndex == currentStep, sequence.step(stepIndex).gate());
    }
}

void LedPainter::drawStepIndex(Leds &leds, int index) {
    for (int step = 0; step < 16; ++step) {
        leds.set(MatrixMap::fromStep(step), step == index, step == index);
    }
}

void LedPainter::drawSelectedPage(Leds &leds, int page) {
    auto setLed = [&] (int code) {
        bool selected = page == code;
        leds.set(code, selected, selected);
        leds.mask(code);
    };

    for (int i = 0; i < 8; ++i) {
        setLed(MatrixMap::fromTrack(i));
    }
    for (int i = 0; i < 16; ++i) {
        setLed(MatrixMap::fromStep(i));
    }
    setLed(Key::Code::Pattern);
    setLed(Key::Code::Performer);
    setLed(Key::Code::Bpm);
}

void LedPainter::drawSelectedSequencePage(Leds &leds, int page) {
    leds.set(Key::Code::Left, page == 0, page == 0);
    leds.set(Key::Code::Right, page == 1, page == 1);
    leds.set(Key::Code::Page, page == 2, page == 2);
    leds.set(Key::Code::Shift, page == 3, page == 3);
}

void LedPainter::drawSelectedPattern(Leds &leds, int activePattern, int requestedPattern) {
    for (int i = 0; i < 16; ++i) {
        leds.set(MatrixMap::fromStep(i), i == activePattern, i == requestedPattern);
    }
}

void LedPainter::drawSelectedPatterns(Leds &leds, uint16_t activePatterns, uint16_t requestedPatterns) {
    for (int i = 0; i < 16; ++i) {
        leds.set(MatrixMap::fromStep(i), activePatterns & (1<<i), requestedPatterns & (1<<i));
    }
}

void LedPainter::drawMutes(Leds &leds, uint8_t activeMutes, uint8_t requestedMutes) {
    for (int i = 0; i < 8; ++i) {
        leds.set(MatrixMap::fromStep(i), requestedMutes & (1<<i), activeMutes & (1<<i));
    }
}

void LedPainter::drawFills(Leds &leds, uint8_t activeFills) {
    for (int i = 0; i < 8; ++i) {
        bool active = activeFills & (1<<i);
        leds.set(MatrixMap::fromStep(i + 8), active, active);
    }
}

void LedPainter::drawMode(Leds &leds, int index, int count) {
    for (int i = 0; i < 8; ++i) {
        leds.set(MatrixMap::fromStep(i), i < count, i == index);
    }
}

void LedPainter::drawValue(Leds &leds, int index, int count) {
    for (int i = 0; i < 8; ++i) {
        leds.set(MatrixMap::fromStep(i + 8), i < count, i == index);
    }
}

void LedPainter::unmaskSteps(Leds &leds) {
    for (int i = 0; i < 16; ++i) {
        leds.unmask(MatrixMap::fromStep(i));
    }
}
