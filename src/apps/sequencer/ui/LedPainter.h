#pragma once

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

    static void drawMode(Leds &leds, int index, int count);
    static void drawValue(Leds &leds, int index, int count);
};
