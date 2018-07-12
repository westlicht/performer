#pragma once

#include "BasePage.h"

#include "engine/MidiPort.h"

#include "core/midi/MidiMessage.h"

class MonitorPage : public BasePage {
public:
    MonitorPage(PageManager &manager, PageContext &context);

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;
    virtual void updateLeds(Leds &leds) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;
    virtual void midi(MidiEvent &event) override;

private:
    void drawCvIn(Canvas &canvas);
    void drawCvOut(Canvas &canvas);
    void drawMidi(Canvas &canvas);
    void drawStats(Canvas &canvas);

    enum class Mode : uint8_t {
        CvIn,
        CvOut,
        Midi,
        Stats,
    };

    Mode _mode = Mode::CvIn;
    MidiMessage _lastMidiMessage;
    MidiPort _lastMidiMessagePort;
    uint32_t _lastMidiMessageTicks = -1;
};
