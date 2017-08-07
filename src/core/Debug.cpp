#include "Debug.h"

#include "drivers/Console.h"
#include "stb/stb_sprintf.h"

#if CONFIG_ENABLE_DEBUG
static char *dbg_write(char *buf, void *user, int len) {
    Console::write(buf, len);
    return buf;
}

void dbg_printf(char const *fmt, ...) {
    va_list va;
    char buf[CONFIG_PRINTF_BUFFER];
    va_start(va, fmt);
    stbsp_vsprintfcb(&dbg_write, buf, buf, fmt, va);
    va_end(va);
}

void dbg_assert(bool cond, const char *fmt, ...) {
    va_list va;
    if (!cond) {
        char buf[CONFIG_PRINTF_BUFFER];
        va_start(va, fmt);
        stbsp_vsprintfcb(&dbg_write, buf, buf, fmt, va);
        va_end(va);
        __builtin_trap();
        while (1) {}
    }
}

#endif // CONFIG_ENABLE_DEBUG
