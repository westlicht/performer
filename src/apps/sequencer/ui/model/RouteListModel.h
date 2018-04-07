#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Routing.h"

class RouteListModel : public ListModel {
public:
    enum Item {
        Param,
        Min,
        Max,
        Tracks,
        Source,
        FirstMidi,
        MidiPort = FirstMidi,
        MidiChannel,
        MidiEvent,
        MidiControlNumberOrNote,
        Last
    };

    RouteListModel(Routing::Route &route) :
        _route(route)
    {}

    virtual int rows() const override {
        bool isMidi = _route.source() == Routing::Source::Midi;
        bool hasNoteOrController = _route.midiSource().event() != Routing::MidiSource::Event::PitchBend;
        return isMidi ? (hasNoteOrController ? Last : (int(Last) - 1)) : FirstMidi;
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
        case Param:         return "Parameter";
        case Min:           return "Min";
        case Max:           return "Max";
        case Tracks:        return "Tracks";
        case Source:        return "Source";
        case MidiPort:      return "MIDI Port";
        case MidiChannel:   return "MIDI Channel";
        case MidiEvent:     return "MIDI Event";
        case MidiControlNumberOrNote:
            if (_route.midiSource().isControlEvent()) {
                            return "CC Number";
            } else {
                            return "Note";
            }
        case Last:          break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Param:
            _route.printParam(str);
            break;
        case Min:
            _route.printMin(str);
            break;
        case Max:
            _route.printMax(str);
            break;
        case Tracks:
            _route.printTracks(str);
            break;
        case Source:
            _route.printSource(str);
            break;
        case MidiPort:
            _route.midiSource().printPort(str);
            break;
        case MidiChannel:
            _route.midiSource().printChannel(str);
            break;
        case MidiEvent:
            _route.midiSource().printEvent(str);
            break;
        case MidiControlNumberOrNote:
            if (_route.midiSource().isControlEvent()) {
                _route.midiSource().printControlNumber(str);
            } else {
                _route.midiSource().printNote(str);
            }
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Param:
            _route.editParam(value, shift);
            break;
        case Min:
            _route.editMin(value, shift);
            break;
        case Max:
            _route.editMax(value, shift);
            break;
        case Tracks:
            // handled in RoutePage
            break;
        case Source:
            _route.editSource(value, shift);
            break;
        case MidiPort:
            _route.midiSource().editPort(value, shift);
            break;
        case MidiChannel:
            _route.midiSource().editChannel(value, shift);
            break;
        case MidiEvent:
            _route.midiSource().editEvent(value, shift);
            break;
        case MidiControlNumberOrNote:
            if (_route.midiSource().isControlEvent()) {
                _route.midiSource().editControlNumber(value, shift);
            } else {
                _route.midiSource().editNote(value, shift);
            }
            break;
        case Last:
            break;
        }
    }

    Routing::Route &_route;
};
