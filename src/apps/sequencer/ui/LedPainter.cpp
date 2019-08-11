#include "LedPainter.h"
#include "Leds.h"
#include "MatrixMap.h"
#include "Key.h"
#include "PageKeyMap.h"

#include "engine/Engine.h"

#include "model/PlayState.h"
#include "model/NoteSequence.h"

// color guidelines:
// yellow -> selection
// green -> active
// red -> inactive

void LedPainter::drawTrackGatesAndSelectedTrack(Leds &leds, const Engine &engine, const PlayState &playState, int selectedTrack) {
    bool blink = (os::ticks() % os::time::ms(200)) < os::time::ms(100);

    for (int track = 0; track < 8; ++track) {
        const auto &trackEngine = engine.trackEngine(track);
        const auto &trackState = playState.trackState(track);

        bool activity = trackEngine.activity();
        bool mute = (trackState.hasMuteRequest() && trackState.mute() != trackState.requestedMute()) ? blink : trackEngine.mute();
        bool selected = track == selectedTrack;

        if (selected) {
            if (mute) {
                leds.set(MatrixMap::fromTrack(track), true, !activity);
            } else {
                leds.set(MatrixMap::fromTrack(track), !activity, true);
            }
        } else {
            if (mute) {
                leds.set(MatrixMap::fromTrack(track), !activity, false);
            } else {
                leds.set(MatrixMap::fromTrack(track), false, activity);
            }
        }
    }
}

void LedPainter::drawTrackGates(Leds &leds, const Engine &engine, const PlayState &playState) {
    drawTrackGatesAndSelectedTrack(leds, engine, playState, -1);
}

void LedPainter::drawNoteSequenceGateAndCurrentStep(Leds &leds, const NoteSequence &sequence, int stepOffset, int currentStep) {
    for (int i = 0; i < 16; ++i) {
        int stepIndex = stepOffset + i;
        leds.set(MatrixMap::fromStep(i), stepIndex == currentStep, sequence.step(stepIndex).gate());
    }
}

void LedPainter::drawSelectedPage(Leds &leds, int page) {
    auto setLed = [&] (int code) {
        bool isPageKey = PageKeyMap::isPageKey(code);
        bool selected = page == code;
        leds.set(code, selected, isPageKey);
        leds.mask(code);
    };

    for (int i = 0; i < 8; ++i) {
        setLed(MatrixMap::fromTrack(i));
    }
    for (int i = 0; i < 16; ++i) {
        setLed(MatrixMap::fromStep(i));
    }
    setLed(Key::Pattern);
    setLed(Key::Performer);
    setLed(Key::Tempo);
    setLed(Key::Left);
}

void LedPainter::drawSelectedSequenceSection(Leds &leds, int section) {
    leds.set(Key::Global4, section == 0, section == 0);
    leds.set(Key::Global5, section == 1, section == 1);
    leds.set(Key::Global6, section == 2, section == 2);
    leds.set(Key::Global7, section == 3, section == 3);
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

void LedPainter::drawSelectedQuickEditValue(Leds &leds, int index, int count) {
    for (int i = 0; i < 16; ++i) {
        leds.set(MatrixMap::fromStep(i), i == index, i < count);
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

void LedPainter::drawSongSlot(Leds &leds, uint16_t usedPatterns) {
    for (int i = 0; i < 16; ++ i) {
        bool active = usedPatterns & (1<<i);
        leds.set(MatrixMap::fromStep(i), active, active);
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
