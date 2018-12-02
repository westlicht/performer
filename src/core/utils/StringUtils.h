#pragma once

#include <cstdlib>

class StringUtils {
public:

    static void copy(char *dst, const char *src, size_t len) {
        if (len == 0) {
            return;
        }
        while (len-- > 1 && *src != '\0') {
            *dst++ = *src++;
        }
        *dst = '\0';
    }

};
