#pragma once

#include "SystemConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ENABLE_DEBUG
void dbg_printf(char const *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
# define DBG(_fmt_, ...) dbg_printf(_fmt_ "\n", ##__VA_ARGS__)
typedef void AssertHandler(const char *filename, int line, const char *msg);
void dbg_set_assert_handler(AssertHandler *handler);
void dbg_assert(bool cond, const char *filename, int line, const char *fmt, ...) __attribute__((__format__(__printf__, 4, 5)));
# define ASSERT(_cond_, _fmt_, ...) dbg_assert(_cond_, __FILE__, __LINE__, _fmt_ "\n", ##__VA_ARGS__)
#else // CONFIG_ENABLE_DEBUG
# define DBG(_fmt_, ...)
# define ASSERT(_conf_, _fmt_, ...)
#endif // CONFIG_ENABLE_DEBUG

#ifdef __cplusplus
}
#endif
