#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/HarmonySequence.h"
#include "model/Scale.h"

class HarmonySequenceStepListModel : public ListModel {
public:
    enum Item {
        Length,
        ChordRoot,
        ChordQuality,
        ChordVoicing,
        Last
    };

    HarmonySequenceStepListModel()
    {}

    void setStep(HarmonySequence::Step *step) {
        _step = step;
    }

    virtual int rows() const override {
        return _step ? Last : 0;
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
    static const char *itemName(Item item) {
        switch (item) {
        case Length:        return "Length";
        case ChordRoot:     return "Chord Root";
        case ChordQuality:  return "Chord Quality";
        case ChordVoicing:  return "Chord Voicing";
        case Last:          break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Length:
            _step->printLength(str);
            break;
        case ChordRoot:
            _step->printChordRoot(str);
            break;
        case ChordQuality:
            _step->printChordQuality(str);
            break;
        case ChordVoicing:
            _step->printChordVoicing(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Length:
            _step->editLength(value, shift);
            break;
        case ChordRoot:
            _step->editChordRoot(value, shift);
            break;
        case ChordQuality:
            _step->editChordQuality(value, shift);
            break;
        case ChordVoicing:
            _step->editChordVoicing(value, shift);
            break;
        case Last:
            break;
        }
    }

    HarmonySequence::Step *_step;
};
