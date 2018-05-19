#include "Config.h"

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_MIN 128
#define STB_SPRINTF_NOFLOAT
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include "stb_sprintf.h"
#pragma GCC diagnostic pop
