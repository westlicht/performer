#pragma once

#include "ListModel.h"

#include "model/Routing.h"

class RoutableListModel : public ListModel {
public:
    virtual Routing::Target routingTarget(int row) const = 0;
};
