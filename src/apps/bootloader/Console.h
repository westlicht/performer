#pragma once

#include "lib/stb_sprintf.h"

#include <cstdlib>

class Console {
public:
    static void init();
    static void deinit();

    static void write(char c);
    static void write(const char *s);
    static void write(const char *s, size_t length);

private:
    static void send(char c);
};

extern "C" {

void printf(char const *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));

} // extern "C"

#define sprintf stbsp_sprintf
#define snprintf stbsp_snprintf
