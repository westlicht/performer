#pragma once

#include "ListModel.h"

#include "model/Track.h"
#include "model/Routing.h"

class TrackListModel : public ListModel {
public:
    virtual void setTrack(Track &track) = 0;

    virtual Routing::Param routingParam(int row) const = 0;
};
