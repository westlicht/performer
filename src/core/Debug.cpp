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

static AssertHandler *assert_handler;

void dbg_set_assert_handler(AssertHandler *handler) {
    assert_handler = handler;
}

void dbg_assert(bool cond, const char *filename, int line, const char *fmt, ...) {
    va_list va;
    if (!cond) {
        if (filename) {
            filename += SOURCE_PATH_SIZE;
            dbg_printf("ASSERT in %s:%d\n", filename, line);
        } else {
            dbg_printf("ASSERT\n");
        }
        char buf[CONFIG_PRINTF_BUFFER];
        va_start(va, fmt);
        stbsp_vsprintfcb(&dbg_write, buf, buf, fmt, va);
        va_end(va);
        if (assert_handler) {
            char msg[128];
            stbsp_vsnprintf(msg, sizeof(msg), fmt, va);
            assert_handler(filename, line, msg);
        }
        __builtin_trap();
        while (1) {}
    }
}

#endif // CONFIG_ENABLE_DEBUG
