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
        switch (Item(row)) {
        case SlideTime:
            return Routing::Target::SlideTime;
        case Transpose:
            return Routing::Target::Transpose;
        default:
            return Routing::Target::None;
        }
    }

private:
    enum Item {
        Source,
        Voices,
        VoiceConfig,
        NotePriority,
        LowNote,
        HighNote,
        PitchBendRange,
        ModulationRange,
        Retrigger,
        SlideTime,
        Transpose,
        ArpeggiatorEnabled,
        ArpeggiatorHold,
        ArpeggiatorMode,
        ArpeggiatorDivisor,
        ArpeggiatorGateLength,
        ArpeggiatorOctaves,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Source:                return "Source";
        case Voices:                return "Voices";
        case VoiceConfig:           return "Voice Config";
        case NotePriority:          return "Note Priority";
        case LowNote:               return "Low Note";
        case HighNote:              return "High Note";
        case PitchBendRange:        return "Pitch Bend";
        case ModulationRange:       return "Mod Range";
        case Retrigger:             return "Retrigger";
        case SlideTime:             return "Slide Time";
        case Transpose:             return "Transpose";
        case ArpeggiatorEnabled:    return "Arpeggiator";
        case ArpeggiatorHold:       return "Hold";
        case ArpeggiatorMode:       return "Mode";
        case ArpeggiatorDivisor:    return "Divisor";
        case ArpeggiatorGateLength: return "Gate Length";
        case ArpeggiatorOctaves:    return "Octaves";
        case Last:                  break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        const auto &arpeggiator = _track->arpeggiator();

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
        case NotePriority:
            _track->printNotePriority(str);
            break;
        case LowNote:
            _track->printLowNote(str);
            break;
        case HighNote:
            _track->printHighNote(str);
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
        case SlideTime:
            _track->printSlideTime(str);
            break;
        case Transpose:
            _track->printTranspose(str);
            break;
        case ArpeggiatorEnabled:
            arpeggiator.printEnabled(str);
            break;
        case ArpeggiatorHold:
            arpeggiator.printHold(str);
            break;
        case ArpeggiatorMode:
            arpeggiator.printMode(str);
            break;
        case ArpeggiatorDivisor:
            arpeggiator.printDivisor(str);
            break;
        case ArpeggiatorGateLength:
            arpeggiator.printGateLength(str);
            break;
        case ArpeggiatorOctaves:
            arpeggiator.printOctaves(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        auto &arpeggiator = _track->arpeggiator();

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
        case NotePriority:
            _track->editNotePriority(value, shift);
            break;
        case LowNote:
            _track->editLowNote(value, shift);
            break;
        case HighNote:
            _track->editHighNote(value, shift);
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
        case SlideTime:
            _track->editSlideTime(value, shift);
            break;
        case Transpose:
            _track->editTranspose(value, shift);
            break;
        case ArpeggiatorEnabled:
            arpeggiator.editEnabled(value, shift);
            break;
        case ArpeggiatorHold:
            arpeggiator.editHold(value, shift);
            break;
        case ArpeggiatorMode:
            arpeggiator.editMode(value, shift);
            break;
        case ArpeggiatorDivisor:
            arpeggiator.editDivisor(value, shift);
            break;
        case ArpeggiatorGateLength:
            arpeggiator.editGateLength(value, shift);
            break;
        case ArpeggiatorOctaves:
            arpeggiator.editOctaves(value, shift);
            break;
        case Last:
            break;
        }
    }

    MidiCvTrack *_track;
};
