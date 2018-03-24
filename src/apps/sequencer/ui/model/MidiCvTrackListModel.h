#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/MidiCvTrack.h"

class MidiCvTrackListModel : public ListModel {
public:
    void setTrack(MidiCvTrack &track) {
        _track = &track;
    }

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
        Port,
        Channel,
        Voices,
        VoiceConfig,
        PitchBendRange,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Port:              return "MIDI Port";
        case Channel:           return "MIDI Channel";
        case Voices:            return "Voices";
        case VoiceConfig:       return "Voice Config";
        case PitchBendRange:    return "Pitch Bend";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Port:
            _track->printPort(str);
            break;
        case Channel:
            _track->printChannel(str);
            break;
        case Voices:
            _track->printVoices(str);
            break;
        case VoiceConfig:
            _track->printVoiceConfig(str);
            break;
        case PitchBendRange:
            _track->printPitchBendRange(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Port:
            _track->editPort(value, shift);
            break;
        case Channel:
            _track->editChannel(value, shift);
            break;
        case Voices:
            _track->editVoices(value, shift);
            break;
        case VoiceConfig:
            _track->editVoiceConfig(value, shift);
            break;
        case PitchBendRange:
            _track->editPitchBendRange(value, shift);
        case Last:
            break;
        }
    }

    MidiCvTrack *_track;
};
