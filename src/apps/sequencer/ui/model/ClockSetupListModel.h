#pragma once

#include "ListModel.h"

#include "ui/utils/AdjustUtils.h"

#include "model/ClockSetup.h"

class ClockSetupListModel : public ListModel {
public:
    ClockSetupListModel(ClockSetup &clockSetup) :
        _clockSetup(clockSetup)
    {}

    virtual int rows() const override {
        return Last;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    virtual void edit(int row, int column, int value) override {
        if (column == 1) {
            editValue(Item(row), value);
        }
    }

private:
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
        Last
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

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Mode:
            str(ClockSetup::modeName(_clockSetup.mode()));
            break;
        case ClockInputPPQN:
            str("%d", _clockSetup.clockInputPPQN());
            break;
        case ClockInputMode:
            str(ClockSetup::clockModeName(_clockSetup.clockInputMode()));
            break;
        case ClockOutputPPQN:
            str("%d", _clockSetup.clockOutputPPQN());
            break;
        case ClockOutputPulse:
            str("%dms", _clockSetup.clockOutputPulse());
            break;
        case ClockOutputMode:
            str(ClockSetup::clockModeName(_clockSetup.clockOutputMode()));
            break;
        case MidiRx:
            str("%s", _clockSetup.midiRx() ? "yes" : "no");
            break;
        case MidiTx:
            str("%s", _clockSetup.midiTx() ? "yes" : "no");
            break;
        case UsbRx:
            str("%s", _clockSetup.usbRx() ? "yes" : "no");
            break;
        case UsbTx:
            str("%s", _clockSetup.usbTx() ? "yes" : "no");
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value) {
        switch (item) {
        case Mode:
            _clockSetup.setMode(adjustedEnum(_clockSetup.mode(), value));
            break;
        case ClockInputPPQN:
            _clockSetup.setClockInputPPQN(_clockSetup.clockInputPPQN() + value);
            break;
        case ClockInputMode:
            _clockSetup.setClockInputMode(adjustedEnum(_clockSetup.clockInputMode(), value));
            break;
        case ClockOutputPPQN:
            _clockSetup.setClockOutputPPQN(_clockSetup.clockOutputPPQN() + value);
            break;
        case ClockOutputPulse:
            _clockSetup.setClockOutputPulse(_clockSetup.clockOutputPulse() + value);
            break;
        case ClockOutputMode:
            _clockSetup.setClockOutputMode(adjustedEnum(_clockSetup.clockOutputMode(), value));
            break;
        case MidiRx:
            _clockSetup.setMidiRx(value > 0);
            break;
        case MidiTx:
            _clockSetup.setMidiTx(value > 0);
            break;
        case UsbRx:
            _clockSetup.setUsbRx(value > 0);
            break;
        case UsbTx:
            _clockSetup.setUsbTx(value > 0);
            break;
        case Last:
            break;
        }
    }

    ClockSetup &_clockSetup;
};
