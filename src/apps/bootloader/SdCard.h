#pragma once

#include <cstddef>
#include <cstdint>

class SdCard {
public:
    static void init();
    static void deinit();

    static bool available();

    static bool read(uint8_t *buf, uint32_t sector, uint8_t count);

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

    static bool cardDetect();

    static void powerOn();
    static void powerOff();

    static void sendCommand(uint32_t cmd, uint32_t arg);
    static Error commandResult();
    static Error sendCommandWait(uint32_t cmd, uint32_t arg);
    static Error sendCommandRetry(uint32_t cmd, uint32_t arg, int maxRetries = 5);
    static Error sendAppCommand(uint32_t cmd, uint32_t arg, int maxRetries = 5);

    static bool initCard();
    static bool waitDataReady();

    static bool readBlock(uint32_t address, void *buffer);

    static bool _initialized;
    static CardInfo _cardInfo;
};
