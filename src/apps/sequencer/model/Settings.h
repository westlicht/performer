#pragma once

#include "Serialize.h"

class Settings {
public:

    // Serizliation

    void write(WriteContext &context) const;
    void read(ReadContext &context);

};
