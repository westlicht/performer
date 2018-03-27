#pragma once

#include "Config.h"

#include "TrackListModel.h"

#include "model/NoteTrack.h"

class NoteTrackListModel : public TrackListModel {
public:
    virtual void setTrack(Track &track) override {
        _track = &track.noteTrack();
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
        case Transpose:
            return Routing::Param::TrackTranspose;
        case Rotate:
            return Routing::Param::TrackRotate;
        default:
            return Routing::Param::Last;
        }
    }

private:
    enum Item {
        PlayMode,
        FillMode,
        Transpose,
        Rotate,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case PlayMode:  return "Play Mode";
        case FillMode:  return "Fill Mode";
        case Transpose: return "Transpose";
        case Rotate:    return "Rotate";
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
        case Transpose:
            _track->printTranspose(str);
            break;
        case Rotate:
            _track->printRotate(str);
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
        case Transpose:
            _track->editTranspose(value, shift);
            break;
        case Rotate:
            _track->editRotate(value, shift);
            break;
        case Last:
            break;
        }
    }

    NoteTrack *_track;
};
