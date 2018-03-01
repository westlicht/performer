#pragma once

#include "Config.h"

#include "ListModel.h"

#include "ui/utils/AdjustUtils.h"

#include "model/TrackSetup.h"

class TrackSetupListModel : public ListModel {
public:
    const TrackSetup &trackSetup() const { return _trackSetup; }
    void setTrackSetup(const TrackSetup &trackSetup) { _trackSetup = trackSetup; }

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

    virtual void edit(int row, int column, int value) override {
        if (column == 1) {
            editValue(Item(row), value);
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
            str(Types::trackModeName(_trackSetup.trackMode()));
            break;
        case PlayMode:
            str(TrackSetup::playModeName(_trackSetup.playMode()));
            break;
        case FillMode:
            str(TrackSetup::fillModeName(_trackSetup.fillMode()));
            break;
        case LinkTrack:
            if (_trackSetup.linkTrack() == -1) {
                str("None");
            } else {
                str("Track%d", _trackSetup.linkTrack() + 1);
            }
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value) {
        switch (item) {
        case TrackMode:
            _trackSetup.setTrackMode(adjustedEnum(_trackSetup.trackMode(), value));
            break;
        case PlayMode:
            _trackSetup.setPlayMode(adjustedEnum(_trackSetup.playMode(), value));
            break;
        case FillMode:
            _trackSetup.setFillMode(adjustedEnum(_trackSetup.fillMode(), value));
            break;
        case LinkTrack:
            _trackSetup.setLinkTrack(clamp(_trackSetup.linkTrack() + value, -1, CONFIG_TRACK_COUNT - 1));
            break;
        case Last:
            break;
        }
    }

    TrackSetup _trackSetup;
};
