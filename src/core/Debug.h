#pragma once

#include "Config.h"

#include "drivers/DebugPin.h"

#include <cstdio>

#if ENABLE_DEBUG
# define DBG(_fmt_, ...) printf(_fmt_ "\r\n", ##__VA_ARGS__)
# define DBG_LO() DebugPin::low()
# define DBG_HI() DebugPin::high()
#else // ENABLE_DEBUG
# define DBG(_fmt_, ...)
# define DBG_LO()
# define DBG_HI()
#endif // ENABLE_DEBUG
