#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/MidiOutput.h"

class OutputListModel : public ListModel {
public:
    enum Item {
        Target,
        Event,
        Last,
    };

    enum NoteItem {
        GateSource = Last,
        NoteSource,
        VelocitySource,
        LastNoteItem,
    };

    enum ControlChangeItem {
        ControlNumber = Last,
        ControlSource,
        LastControlChangeItem,
    };

    OutputListModel(MidiOutput::Output &output) :
        _output(output)
    {}

    virtual int rows() const override {
        switch (_output.event()) {
        case MidiOutput::Output::Event::None:
            return Last;
        case MidiOutput::Output::Event::Note:
            return LastNoteItem;
        case MidiOutput::Output::Event::ControlChange:
            return LastControlChangeItem;
        default:
            return Last;
        }
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
    const char *itemName(Item item) const {
        switch (item) {
        case Target:        return "Target";
        case Event:         return "Event";
        case Last:          break;
        }

        if (_output.isNoteEvent()) {
            switch (NoteItem(item)) {
            case GateSource:    return "Gate Source";
            case NoteSource:    return "Note Source";
            case VelocitySource:return "Vel. Source";
            case LastNoteItem:  break;
            }
        } else if (_output.isControlChangeEvent()) {
            switch (ControlChangeItem(item)) {
            case ControlNumber: return "Control Number";
            case ControlSource: return "Control Source";
            case LastControlChangeItem: break;
            }
        }

        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Target:
            _output.target().print(str);
            break;
        case Event:
            _output.printEvent(str);
            break;
        case Last:
            break;
        }

        if (_output.isNoteEvent()) {
            switch (NoteItem(item)) {
            case GateSource:
                _output.printGateSource(str);
                break;
            case NoteSource:
                _output.printNoteSource(str);
                break;
            case VelocitySource:
                _output.printVelocitySource(str);
                break;
            case LastNoteItem:
            break;
            }
        } else if (_output.isControlChangeEvent()) {
            switch (ControlChangeItem(item)) {
            case ControlNumber:
                _output.printControlNumber(str);
                break;
            case ControlSource:
                _output.printControlSource(str);
                break;
            case LastControlChangeItem:
                break;
            }
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Target:
            _output.target().edit(value, shift);
            break;
        case Event:
            _output.editEvent(value, shift);
            break;
        case Last:
            break;
        }

        if (_output.isNoteEvent()) {
            switch (NoteItem(item)) {
            case GateSource:
                _output.editGateSource(value, shift);
                break;
            case NoteSource:
                _output.editNoteSource(value, shift);
                break;
            case VelocitySource:
                _output.editVelocitySource(value, shift);
                break;
            case LastNoteItem:
            break;
            }
        } else if (_output.isControlChangeEvent()) {
            switch (ControlChangeItem(item)) {
            case ControlNumber:
                _output.editControlNumber(value, shift);
                break;
            case ControlSource:
                _output.editControlSource(value, shift);
                break;
            case LastControlChangeItem:
                break;
            }
        }
    }

    MidiOutput::Output &_output;
};
