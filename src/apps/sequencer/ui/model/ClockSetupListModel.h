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
            _clockSetup.printMode(str);
            break;
        case ClockInputPPQN:
            _clockSetup.printClockInputPPQN(str);
            break;
        case ClockInputMode:
            _clockSetup.printClockInputMode(str);
            break;
        case ClockOutputPPQN:
            _clockSetup.printClockOutputPPQN(str);
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
        case ClockInputPPQN:
            _clockSetup.editClockInputPPQN(value, shift);
            break;
        case ClockInputMode:
            _clockSetup.editClockInputMode(value, shift);
            break;
        case ClockOutputPPQN:
            _clockSetup.editClockOutputPPQN(value, shift);
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
        case ClockInputPPQN:
            return _clockSetup.clockInputPPQN();
        case ClockInputMode:
            return int(_clockSetup.clockInputMode());
        case ClockOutputPPQN:
            return _clockSetup.clockOutputPPQN();
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
            return _clockSetup.setMode(ClockSetup::Mode(index));
        case ClockInputPPQN:
            return _clockSetup.setClockInputPPQN(index);
        case ClockInputMode:
            return _clockSetup.setClockInputMode(ClockSetup::ClockMode(index));
        case ClockOutputPPQN:
            return _clockSetup.setClockOutputPPQN(index);
        case ClockOutputPulse:
            return _clockSetup.setClockOutputPulse(index + 1);
        case ClockOutputMode:
            return _clockSetup.setClockOutputMode(ClockSetup::ClockMode(index));
        case MidiRx:
            return _clockSetup.setMidiRx(bool(index));
        case MidiTx:
            return _clockSetup.setMidiTx(bool(index));
        case UsbRx:
            return _clockSetup.setUsbRx(bool(index));
        case UsbTx:
            return _clockSetup.setUsbTx(bool(index));
        case Last:
            break;
        }
    }

    ClockSetup &_clockSetup;
};
