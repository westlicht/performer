#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Track.h"

class TrackSetupListModel : public ListModel {
public:
    const Track &track() const { return _track; }
    void setTrack(const Track &track) { _track = track; }

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
        TrackMode,
        PlayMode,
        FillMode,
        LinkTrack,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case TrackMode: return "Track Mode";
        case PlayMode:  return "Play Mode";
        case FillMode:  return "Fill Mode";
        case LinkTrack: return "Link Track";
        case Last:      break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case TrackMode:
            _track.printTrackMode(str);
            break;
        case PlayMode:
            _track.printPlayMode(str);
            break;
        case FillMode:
            _track.printFillMode(str);
            break;
        case LinkTrack:
            _track.printLinkTrack(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case TrackMode:
            _track.editTrackMode(value, shift);
            break;
        case PlayMode:
            _track.editPlayMode(value, shift);
            break;
        case FillMode:
            _track.editFillMode(value, shift);
            break;
        case LinkTrack:
            _track.editLinkTrack(value, shift);
            break;
        case Last:
            break;
        }
    }

    Track _track;
};
