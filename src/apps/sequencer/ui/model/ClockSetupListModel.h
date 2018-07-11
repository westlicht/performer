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

    virtual int indexed(int row) const override {
        return indexedValue(Item(row));
    }

    virtual void setIndexed(int row, int index) override {
        setIndexedValue(Item(row), index);
    }

private:
    enum Item {
        Mode,
        ShiftMode,
        ClockInputDivisor,
        ClockInputMode,
        ClockOutputDivisor,
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

    int indexedValue(Item item) const {
        switch (item) {
        case Mode:
            return int(_clockSetup.mode());
        case ShiftMode:
            return int(_clockSetup.shiftMode());
        case ClockInputDivisor:
            return _clockSetup.indexedClockInputDivisor();
        case ClockInputMode:
            return int(_clockSetup.clockInputMode());
        case ClockOutputDivisor:
            return _clockSetup.indexedClockOutputDivisor();
        case ClockOutputPulse:
            return _clockSetup.clockOutputPulse() - 1;
        case ClockOutputMode:
            return int(_clockSetup.clockOutputMode());
        case MidiRx:
            return int(_clockSetup.midiRx());
        case MidiTx:
            return int(_clockSetup.midiTx());
        case UsbRx:
            return int(_clockSetup.usbRx());
        case UsbTx:
            return int(_clockSetup.usbTx());
        case Last:
            break;
        }
        return -1;
    }

    void setIndexedValue(Item item, int index) {
        switch (item) {
        case Mode:
            _clockSetup.setMode(ClockSetup::Mode(index));
        case ShiftMode:
            _clockSetup.setShiftMode(ClockSetup::ShiftMode(index));
        case ClockInputDivisor:
            _clockSetup.setIndexedClockInputDivisor(index);
        case ClockInputMode:
            _clockSetup.setClockInputMode(ClockSetup::ClockInputMode(index));
        case ClockOutputDivisor:
            _clockSetup.setIndexedClockOutputDivisor(index);
        case ClockOutputPulse:
            _clockSetup.setClockOutputPulse(index + 1);
        case ClockOutputMode:
            _clockSetup.setClockOutputMode(ClockSetup::ClockOutputMode(index));
        case MidiRx:
            _clockSetup.setMidiRx(bool(index));
        case MidiTx:
            _clockSetup.setMidiTx(bool(index));
        case UsbRx:
            _clockSetup.setUsbRx(bool(index));
        case UsbTx:
            _clockSetup.setUsbTx(bool(index));
        case Last:
            break;
        }
    }

    ClockSetup &_clockSetup;
};
