#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Routing.h"

#include <array>

class RoutingListModel : public ListModel {
public:
    RoutingListModel(Routing &routing) :
        _routing(routing)
    {}

    virtual int rows() const override {
        return CONFIG_ROUTE_COUNT;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            str("Route%d", row + 1);
        } else if (column == 1) {
            const auto &route = _routing.route(row);
            str(route.active() ? "used" : "unused");
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            // _routing.track(row).editLinkTrack(value, shift);
        }
    }

private:
    Routing &_routing;
};
