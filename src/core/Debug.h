#pragma once

#include "SystemConfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ENABLE_DEBUG
void dbg_printf(char const *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
# define DBG(_fmt_, ...) dbg_printf(_fmt_ "\n", ##__VA_ARGS__)
void dbg_assert(bool cond, const char *fmt, ...) __attribute__((__format__(__printf__, 2, 3)));
# define ASSERT_STR_HELPER(x) #x
# define ASSERT_STR(x) ASSERT_STR_HELPER(x)
# define ASSERT(_cond_, _fmt_, ...) dbg_assert(_cond_, "ASSERT in " __FILE__ ":" ASSERT_STR(__LINE__) "\n" _fmt_ "\n", ##__VA_ARGS__)
#else // CONFIG_ENABLE_DEBUG
# define DBG(_fmt_, ...)
#endif // CONFIG_ENABLE_DEBUG

#ifdef __cplusplus
}
#endif
