#pragma once

#include "VersionTag.h"

#include <cstdlib>

class UpdateFile {
public:
    static bool open(VersionTag &version, size_t &size, uint8_t md5[16], char *errorStr, size_t errorLen);
    static bool rewind(char *errorStr, size_t errorLen);
    static bool read(void *readBuf, size_t readLen, char *errorStr, size_t errorLen);
};
