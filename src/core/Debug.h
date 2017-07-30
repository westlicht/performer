#pragma once

#include "Config.h"

#include "drivers/DebugPin.h"

#if ENABLE_DEBUG
void dbg_printf(char const *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
# define DBG(_fmt_, ...) dbg_printf(_fmt_ "\r\n", ##__VA_ARGS__)
# define DBG_LO() DebugPin::low()
# define DBG_HI() DebugPin::high()
#else // ENABLE_DEBUG
# define DBG(_fmt_, ...)
# define DBG_LO()
# define DBG_HI()
#endif // ENABLE_DEBUG
