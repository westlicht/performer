#include "LedPainter.h"
#include "Leds.h"
#include "MatrixMap.h"

#include "engine/Engine.h"

#include "model/Sequence.h"

void LedPainter::drawTracksGateAndSelected(Leds &leds, const Engine &engine, int selectedTrack) {
    for (int track = 0; track < 8; ++track) {
        leds.set(MatrixMap::fromTrack(track), engine.track(track).gateOutput(), track == selectedTrack);
    }
}

void LedPainter::drawTracksGateAndMuted(Leds &leds, const Engine &engine) {
    for (int track = 0; track < 8; ++track) {
        leds.set(MatrixMap::fromTrack(track), engine.track(track).gate(), engine.track(track).muted());
    }
}

void LedPainter::drawSequenceGateAndCurrentStep(Leds &leds, const Sequence &sequence, int currentStep) {
    for (int step = 0; step < 16; ++step) {
        leds.set(MatrixMap::fromStep(step), step == currentStep, sequence.step(step).active());
    }
}
