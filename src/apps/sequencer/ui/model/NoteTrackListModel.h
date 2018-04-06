#pragma once

#include "Config.h"

#include "ParamListModel.h"

#include "model/NoteTrack.h"

class NoteTrackListModel : public ParamListModel {
public:
    void setTrack(NoteTrack &track) {
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

    virtual Routing::Param routingParam(int row) const override {
        switch (Item(row)) {
        case SlideTime:
            return Routing::Param::TrackSlideTime;
        case Octave:
            return Routing::Param::TrackOctave;
        case Transpose:
            return Routing::Param::TrackTranspose;
        case Rotate:
            return Routing::Param::TrackRotate;
        case StepProbabilityBias:
            return Routing::Param::TrackStepProbabilityBias;
        case StepLengthBias:
            return Routing::Param::TrackStepLengthBias;
        default:
            return Routing::Param::None;
        }
    }

private:
    enum Item {
        PlayMode,
        FillMode,
        SlideTime,
        Octave,
        Transpose,
        Rotate,
        StepProbabilityBias,
        StepLengthBias,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case PlayMode:  return "Play Mode";
        case FillMode:  return "Fill Mode";
        case SlideTime: return "Slide Time";
        case Octave:    return "Octave";
        case Transpose: return "Transpose";
        case Rotate:    return "Rotate";
        case StepProbabilityBias: return "Step P. Bias";
        case StepLengthBias: return "Step L. Bias";
        case Last:      break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case PlayMode:
            _track->printPlayMode(str);
            break;
        case FillMode:
            _track->printFillMode(str);
            break;
        case SlideTime:
            _track->printSlideTime(str);
            break;
        case Octave:
            _track->printOctave(str);
            break;
        case Transpose:
            _track->printTranspose(str);
            break;
        case Rotate:
            _track->printRotate(str);
            break;
        case StepProbabilityBias:
            _track->printStepProbabilityBias(str);
            break;
        case StepLengthBias:
            _track->printStepLengthBias(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case PlayMode:
            _track->editPlayMode(value, shift);
            break;
        case FillMode:
            _track->editFillMode(value, shift);
            break;
        case SlideTime:
            _track->editSlideTime(value, shift);
            break;
        case Octave:
            _track->editOctave(value, shift);
            break;
        case Transpose:
            _track->editTranspose(value, shift);
            break;
        case Rotate:
            _track->editRotate(value, shift);
            break;
        case StepProbabilityBias:
            _track->editStepProbabilityBias(value, shift);
            break;
        case StepLengthBias:
            _track->editStepLengthBias(value, shift);
            break;
        case Last:
            break;
        }
    }

    NoteTrack *_track;
};
