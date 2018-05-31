#pragma once

#include "SystemConfig.h"

#include <libopencm3/stm32/flash.h>

#include <cstdint>

class Flash {
public:
    static void unlock() {
        flash_unlock();
    }

    static void lock() {
        flash_lock();
    }

    static void eraseSector(uint32_t sector) {
        flash_erase_sector(sector, 2);
        flash_wait_for_last_operation();
    }

    static void program(uint32_t address, uint32_t data) {
        flash_program_word(address, data);
        flash_wait_for_last_operation();
    }
};
