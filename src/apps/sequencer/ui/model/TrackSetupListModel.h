#pragma once

#include "ListModel.h"

#include "ui/utils/AdjustUtils.h"

#include "model/Track.h"

class TrackSetupListModel : public ListModel {
public:
    const Track &track() const { return _track; }
    void setTrack(const Track &track) { _track = track; }

    virtual int rows() const override {
        return 1;
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

    virtual void edit(int row, int column, int value) override {
        if (column == 1) {
            editValue(Item(row), value);
        }
    }

private:
    enum Item {
        Mode,
        Last,
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Mode:              return "Mode";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Mode:
            str(Track::modeName(_track.mode()));
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value) {
        switch (item) {
        case Mode:
            _track.setMode(adjustedEnum(_track.mode(), value));
            break;
        case Last:
            break;
        }
    }

    Track _track;
};
