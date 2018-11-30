#pragma once

#include "Error.h"

#include "drivers/SdCard.h"

#include <cstddef>
#include <cstdint>

namespace fs {

class Volume {
public:
    Volume(SdCard &sdcard);
    ~Volume();

    SdCard &sdcard() { return _sdcard; }

    bool available();

    Error format();

    Error mount();
    Error unmount();

    Error stats(size_t *total, size_t *free) const;
    size_t sizeTotal() const;
    size_t sizeFree() const;

private:
    SdCard &_sdcard;
    FATFS _fs;
};

} // namespace fs
