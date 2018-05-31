#pragma once

#include "drivers/Flash.h"

class FlashReader {
public:
    FlashReader(uint32_t address) :
        _address(reinterpret_cast<const uint8_t *>(address))
    {
    }

    void read(void *data, size_t len) {
#ifdef PLATFORM_STM32
        memcpy(data, _address, len);
#endif
        _address += len;
    }

private:
    const uint8_t *_address;
};
