#pragma once

#include "Serialize.h"

class Settings {
public:
    void write(WriteContext &context) const;
    void read(ReadContext &context);
};
