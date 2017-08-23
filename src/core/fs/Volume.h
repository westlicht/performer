#pragma once

#include "Error.h"

#include "drivers/SDCard.h"

#include <cstdint>

namespace fs {

class Volume {
public:
    Volume(SDCard &sdcard);

    SDCard &sdcard() { return _sdcard; }

    bool available();

    Error format();

    Error mount();
    Error unmount();

    Error stats(size_t *total, size_t *free) const;
    size_t sizeTotal() const;
    size_t sizeFree() const;

private:
    SDCard &_sdcard;
    FATFS _fs;
};

} // namespace fs
