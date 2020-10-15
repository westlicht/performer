#pragma once

#include "Config.h"

#include "RoutableListModel.h"

#include "model/CurveTrack.h"

class CurveTrackListModel : public RoutableListModel {
public:
    void setTrack(CurveTrack &track) {
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
        case Offset:
            return Routing::Target::Offset;
        case Rotate:
            return Routing::Target::Rotate;
        case ShapeProbabilityBias:
            return Routing::Target::ShapeProbabilityBias;
        case GateProbabilityBias:
            return Routing::Target::GateProbabilityBias;
        default:
            return Routing::Target::None;
        }
    }

private:
    enum Item {
        PlayMode,
        FillMode,
        MuteMode,
        SlideTime,
        Offset,
        Rotate,
        ShapeProbabilityBias,
        GateProbabilityBias,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case PlayMode:              return "Play Mode";
        case FillMode:              return "Fill Mode";
        case MuteMode:              return "Mute Mode";
        case SlideTime:             return "Slide Time";
        case Offset:                return "Offset";
        case Rotate:                return "Rotate";
        case ShapeProbabilityBias:  return "Shape P. Bias";
        case GateProbabilityBias:   return "Gate P. Bias";
        case Last:                  break;
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
        case MuteMode:
            _track->printMuteMode(str);
            break;
        case SlideTime:
            _track->printSlideTime(str);
            break;
        case Offset:
            _track->printOffset(str);
            break;
        case Rotate:
            _track->printRotate(str);
            break;
        case ShapeProbabilityBias:
            _track->printShapeProbabilityBias(str);
            break;
        case GateProbabilityBias:
            _track->printGateProbabilityBias(str);
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
        case MuteMode:
            _track->editMuteMode(value, shift);
            break;
        case SlideTime:
            _track->editSlideTime(value, shift);
            break;
        case Offset:
            _track->editOffset(value, shift);
            break;
        case Rotate:
            _track->editRotate(value, shift);
            break;
        case ShapeProbabilityBias:
            _track->editShapeProbabilityBias(value, shift);
            break;
        case GateProbabilityBias:
            _track->editGateProbabilityBias(value, shift);
            break;
        case Last:
            break;
        }
    }

    CurveTrack *_track;
};
