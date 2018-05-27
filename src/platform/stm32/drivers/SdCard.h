#pragma once

#include "core/Debug.h"

#include <cstddef>
#include <cstdint>

class SdCard {
public:
    void init();

    bool available();

    bool writeProtected();

    size_t sectorCount() const { return _cardInfo.size; }
    size_t sectorSize() const { return 512; }

    bool read(uint8_t *buf, uint32_t sector, uint8_t count);
    bool write(const uint8_t *buf, uint32_t sector, uint8_t count);

    void sync() {
    }

private:
    enum Error {
        Success,
        InProgress,
        Timeout,
        CRCFail,
        Unknown,
    };

    struct CardInfo {
        // Relative Card Address, used when sending certain commands.
        uint16_t rca = 0;
        // If ccs true then the card is a SDHC/SDXC card and uses block based
        // addressing. Otherwise it is an SDSC card and uses byte based addresing.
        bool ccs = false;
        // Size is in 512 byte blocks. Max size is 2T or 0x100000000.
        uint32_t size = 0;
    };

    bool cardDetect() const;

    void powerOn();
    void powerOff();

    void sendCommand(uint32_t cmd, uint32_t arg);
    Error commandResult();
    Error sendCommandWait(uint32_t cmd, uint32_t arg);
    Error sendCommandRetry(uint32_t cmd, uint32_t arg, int maxRetries = 5);
    Error sendAppCommand(uint32_t cmd, uint32_t arg, int maxRetries = 5);

    bool initCard();
    bool waitDataReady();

    bool readBlock(uint32_t address, void *buffer);
    bool writeBlock(uint32_t address, const void *buffer);

    bool _initialized = false;
    CardInfo _cardInfo;
};
