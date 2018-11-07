#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/MidiOutput.h"

class OutputListModel : public ListModel {
public:
    enum Item {
        Target,
        Event,
        FirstParameter,
        GateSource = FirstParameter,
        NoteSource,
        VelocitySource,
        Last,
        ControlNumber = FirstParameter,
        ControlSource,
    };

    OutputListModel(MidiOutput::Output &output) :
        _output(output)
    {}

    virtual int rows() const override {
        switch (_output.event()) {
        case MidiOutput::Output::Event::None:
            return FirstParameter;
        case MidiOutput::Output::Event::Note:
            return FirstParameter + 3;
        case MidiOutput::Output::Event::ControlChange:
            return FirstParameter + 2;
        default:
            return FirstParameter;
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
        // case ControlNumber:
        case GateSource:    return _output.event() == MidiOutput::Output::Event::Note ? "Gate Source" : "Control Number";
        // case ControlSource
        case NoteSource:    return _output.event() == MidiOutput::Output::Event::Note ? "Note Source" : "Control Source";
        case VelocitySource:return "Vel. Source";
        case Last:          break;
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
        // case ControlNumber:
        case GateSource:
            if (_output.event() == MidiOutput::Output::Event::Note) {
                _output.printGateSource(str);
            } else {
                _output.printControlNumber(str);
            }
            break;
        // case ControlSource
        case NoteSource:
            if (_output.event() == MidiOutput::Output::Event::Note) {
                _output.printNoteSource(str);
            } else {
                _output.printControlSource(str);
            }
            break;
        case VelocitySource:
            _output.printVelocitySource(str);
            break;
        case Last:
            break;
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
        // case ControlNumber:
        case GateSource:
            if (_output.event() == MidiOutput::Output::Event::Note) {
                _output.editGateSource(value, shift);
            } else {
                _output.editControlNumber(value, shift);
            }
            break;
        // case ControlSource
        case NoteSource:
            if (_output.event() == MidiOutput::Output::Event::Note) {
                _output.editNoteSource(value, shift);
            } else {
                _output.editControlSource(value, shift);
            }
            break;
        case VelocitySource:
            _output.editVelocitySource(value, shift);
            break;
        case Last:
            break;
        }
    }

    MidiOutput::Output &_output;
};
