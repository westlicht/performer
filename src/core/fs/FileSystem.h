#pragma once

#include "Error.h"
#include "Volume.h"
#include "File.h"
#include "Directory.h"

#include <cstddef>
#include <cstdint>

namespace fs {

void setVolume(Volume *volume);
Volume &volume();

Error mkdir(const char *path);
Error rmdir(const char *path);
Error remove(const char *path);
Error rename(const char *oldPath, const char *newPath);

bool exists(const char *path);

} // namespace fs
