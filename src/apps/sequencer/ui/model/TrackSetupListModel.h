#pragma once

#include "ListModel.h"

#include "model/Track.h"

class TrackSetupListModel : public ListModel {
public:
    virtual int rows() const override { return 10; }
    virtual int columns() const override { return 2; }

    // virtual void requestRow(int row) {}

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            str("Itemg %d", row);
        } else if (column == 1) {
            str("Value %d", row);
        }
    }

    virtual void edit(int row, int column, int value) override {
    }

private:
    Track *_track = nullptr;
};
