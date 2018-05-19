#pragma once

#include <libopencm3/cm3/cortex.h>

class InterruptLock {
public:
    InterruptLock() { cm_disable_interrupts(); }
    ~InterruptLock() { cm_enable_interrupts(); }
};
