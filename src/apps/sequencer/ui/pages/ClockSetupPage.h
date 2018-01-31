#pragma once

#include "MenuPage.h"

class ClockSetupPage : public MenuPage {
public:
    enum Item {
        Mode,
        ClockInputPPQN,
        ClockInputMode,
        ClockOutputPPQN,
        ClockOutputPulse,
        ClockOutputMode,
        MidiRx,
        MidiTx,
        UsbRx,
        UsbTx,
        Last,
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Mode:              return "Mode";
        case ClockInputPPQN:    return "Input PPQN";
        case ClockInputMode:    return "Input Mode";
        case ClockOutputPPQN:   return "Output PPQN";
        case ClockOutputPulse:  return "Output Pulse";
        case ClockOutputMode:   return "Output Mode";
        case MidiRx:            return "MIDI RX";
        case MidiTx:            return "MIDI TX";
        case UsbRx:             return "USB RX";
        case UsbTx:             return "USB TX";
        case Last:              break;
        }
        return nullptr;
    }

    ClockSetupPage(PageManager &manager, PageContext &context);

    virtual void draw(Canvas &canvas) override;

    virtual void encoder(EncoderEvent &event) override;
};
