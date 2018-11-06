#pragma once

#include "ListModel.h"

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

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
        }
    }

private:
    enum Item {
        Mode,
        ShiftMode,
        ClockInputDivisor,
        ClockInputMode,
        ClockOutputDivisor,
        ClockOutputSwing,
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
        case ShiftMode:         return "Shift Mode";
        case ClockInputDivisor: return "Input Divisor";
        case ClockInputMode:    return "Input Mode";
        case ClockOutputDivisor:return "Output Divisor";
        case ClockOutputSwing:  return "Output Swing";
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
            _clockSetup.printMode(str);
            break;
        case ShiftMode:
            _clockSetup.printShiftMode(str);
            break;
        case ClockInputDivisor:
            _clockSetup.printClockInputDivisor(str);
            break;
        case ClockInputMode:
            _clockSetup.printClockInputMode(str);
            break;
        case ClockOutputDivisor:
            _clockSetup.printClockOutputDivisor(str);
            break;
        case ClockOutputSwing:
            _clockSetup.printClockOutputSwing(str);
            break;
        case ClockOutputPulse:
            _clockSetup.printClockOutputPulse(str);
            break;
        case ClockOutputMode:
            _clockSetup.printClockOutputMode(str);
            break;
        case MidiRx:
            _clockSetup.printMidiRx(str);
            break;
        case MidiTx:
            _clockSetup.printMidiTx(str);
            break;
        case UsbRx:
            _clockSetup.printUsbRx(str);
            break;
        case UsbTx:
            _clockSetup.printUsbTx(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Mode:
            _clockSetup.editMode(value, shift);
            break;
        case ShiftMode:
            _clockSetup.editShiftMode(value, shift);
            break;
        case ClockInputDivisor:
            _clockSetup.editClockInputDivisor(value, shift);
            break;
        case ClockInputMode:
            _clockSetup.editClockInputMode(value, shift);
            break;
        case ClockOutputDivisor:
            _clockSetup.editClockOutputDivisor(value, shift);
            break;
        case ClockOutputSwing:
            _clockSetup.editClockOutputSwing(value, shift);
            break;
        case ClockOutputPulse:
            _clockSetup.editClockOutputPulse(value, shift);
            break;
        case ClockOutputMode:
            _clockSetup.editClockOutputMode(value, shift);
            break;
        case MidiRx:
            _clockSetup.editMidiRx(value, shift);
            break;
        case MidiTx:
            _clockSetup.editMidiTx(value, shift);
            break;
        case UsbRx:
            _clockSetup.editUsbRx(value, shift);
            break;
        case UsbTx:
            _clockSetup.editUsbTx(value, shift);
            break;
        case Last:
            break;
        }
    }

    ClockSetup &_clockSetup;
};
