#pragma once

#include "Error.h"
#include "Volume.h"
#include "File.h"

#include <cstdint>

namespace fs {

void setVolume(Volume &volume);
Volume &volume();

// Error mkdir(const char *path);
// Error rmdir(const char *path);
// Error remove(const char *path);

// struct FileInfo {
//     uint32_t size;  // file size
//     uint16_t date;  // last modified date
//     uint16_t time;  // last modified time
//     uint8_t attrib; // aatribute
//     char name[13];  // short file name (8.3 format)
// };

} // namespace fs
