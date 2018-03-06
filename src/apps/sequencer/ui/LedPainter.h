#pragma once

class Leds;
class Engine;
class NoteSequence;

class LedPainter {
public:
    static void drawTracksGateAndSelected(Leds &leds, const Engine &engine, int selectedTrack);
    static void drawTracksGateAndMuted(Leds &leds, const Engine &engine);

    static void drawNoteSequenceGateAndCurrentStep(Leds &leds, const NoteSequence &sequence, int currentStep);

    static void drawSelectedPage(Leds &leds, int page);

};
