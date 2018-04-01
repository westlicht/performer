#include "LedPainter.h"
#include "Leds.h"
#include "MatrixMap.h"
#include "Key.h"

#include "engine/Engine.h"

#include "model/NoteSequence.h"

void LedPainter::drawTracksGateAndSelected(Leds &leds, const Engine &engine, int selectedTrack) {
    for (int track = 0; track < 8; ++track) {
        leds.set(MatrixMap::fromTrack(track), engine.trackEngine(track).activity(), track == selectedTrack);
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
    for (int i = 0; i < 8; ++i) {
        leds.set(MatrixMap::fromTrack(i), false, i == page);
        leds.mask(MatrixMap::fromTrack(i));
    }
    for (int i = 0; i < 16; ++i) {
        leds.set(MatrixMap::fromStep(i), false, (i + 8) == page);
        leds.mask(MatrixMap::fromStep(i));
    }
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
