#include "SystemConfig.h"
#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_MIN CONFIG_PRINTF_BUFFER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include "stb_sprintf.h"
#pragma GCC diagnostic pop
