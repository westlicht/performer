#pragma once

#include "Config.h"

#include "RoutableListModel.h"

#include "model/HarmonyTrack.h"

class HarmonyTrackListModel : public RoutableListModel {
public:
    void setTrack(HarmonyTrack &track) {
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
        default:
            return Routing::Target::None;
        }
    }

private:
    enum Item {
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Last:      break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Last:
            break;
        }
    }

    HarmonyTrack *_track;
};
