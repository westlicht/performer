#pragma once

#include "ListModel.h"

#include "model/Routing.h"

class ParamListModel : public ListModel {
public:
    virtual Routing::Param routingParam(int row) const = 0;
};
