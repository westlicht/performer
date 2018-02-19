#include "LedPainter.h"
#include "Leds.h"
#include "MatrixMap.h"

#include "engine/Engine.h"

#include "model/Sequence.h"

void LedPainter::drawTracksGateAndSelected(Leds &leds, const Engine &engine, int selectedTrack) {
    for (int track = 0; track < 8; ++track) {
        leds.set(MatrixMap::fromTrack(track), engine.trackEngine(track).gateOutput(), track == selectedTrack);
    }
}

void LedPainter::drawTracksGateAndMuted(Leds &leds, const Engine &engine) {
    for (int track = 0; track < 8; ++track) {
        // TODO show muted gates as well
        leds.set(MatrixMap::fromTrack(track), engine.trackEngine(track).gateOutput(), engine.trackEngine(track).muted());
    }
}

void LedPainter::drawNoteSequenceGateAndCurrentStep(Leds &leds, const NoteSequence &sequence, int currentStep) {
    for (int step = 0; step < 16; ++step) {
        leds.set(MatrixMap::fromStep(step), step == currentStep, sequence.step(step).gate());
    }
}
