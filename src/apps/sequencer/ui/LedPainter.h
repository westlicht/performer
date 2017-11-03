#pragma once

class Leds;
class Engine;
class Sequence;

class LedPainter {
public:
    static void drawTracksGateAndSelected(Leds &leds, const Engine &engine, int selectedTrack);
    static void drawTracksGateAndMuted(Leds &leds, const Engine &engine);

    static void drawSequenceGateAndCurrentStep(Leds &leds, const Sequence &sequence, int currentStep);
};
