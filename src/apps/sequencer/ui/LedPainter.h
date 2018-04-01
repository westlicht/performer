#pragma once

#include <cstdint>

class Leds;
class Engine;
class NoteSequence;

class LedPainter {
public:
    static void drawTracksGateAndSelected(Leds &leds, const Engine &engine, int selectedTrack);
    static void drawTracksGateAndMuted(Leds &leds, const Engine &engine);

    static void drawNoteSequenceGateAndCurrentStep(Leds &leds, const NoteSequence &sequence, int stepOffset, int currentStep);

    static void drawStepIndex(Leds &leds, int index);

    static void drawSelectedPage(Leds &leds, int page);
    static void drawSelectedSequencePage(Leds &leds, int page);
    static void drawSelectedPattern(Leds &leds, int activePattern, int requestedPattern);
    static void drawSelectedPatterns(Leds &leds, uint16_t activePatterns, uint16_t requestedPatterns);

    static void drawMutes(Leds &leds, uint8_t activeMutes, uint8_t requestedMutes);

    static void drawMode(Leds &leds, int index, int count);
    static void drawValue(Leds &leds, int index, int count);
};
