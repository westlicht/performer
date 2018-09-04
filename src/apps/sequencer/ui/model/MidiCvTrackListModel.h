#pragma once

#include "Config.h"

#include "RoutableListModel.h"

#include "model/MidiCvTrack.h"

class MidiCvTrackListModel : public RoutableListModel {
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

    virtual Routing::Target routingTarget(int row) const override {
        return Routing::Target::None;
    }

private:
    enum Item {
        Source,
        Voices,
        VoiceConfig,
        PitchBendRange,
        ModulationRange,
        Retrigger,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Source:            return "Source";
        case Voices:            return "Voices";
        case VoiceConfig:       return "Voice Config";
        case PitchBendRange:    return "Pitch Bend";
        case ModulationRange:   return "Mod Range";
        case Retrigger:         return "Retrigger";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Source:
            _track->source().print(str);
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
        case ModulationRange:
            _track->printModulationRange(str);
            break;
        case Retrigger:
            _track->printRetrigger(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Source:
            _track->source().edit(value, shift);
            break;
        case Voices:
            _track->editVoices(value, shift);
            break;
        case VoiceConfig:
            _track->editVoiceConfig(value, shift);
            break;
        case PitchBendRange:
            _track->editPitchBendRange(value, shift);
            break;
        case ModulationRange:
            _track->editModulationRange(value, shift);
            break;
        case Retrigger:
            _track->editRetrigger(value, shift);
            break;
        case Last:
            break;
        }
    }

    MidiCvTrack *_track;
};
