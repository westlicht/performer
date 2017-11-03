#pragma once

#include "Error.h"
#include "FileInfo.h"

#include "ff/ff.h"

#include <cstddef>
#include <cstdint>

namespace fs {

class Directory {
public:
    Directory(const char *path) {
        _error = Error(f_opendir(&_dir, path));
    }

    ~Directory() {
        f_closedir(&_dir);
    }

    const FileInfo &info() const { return _info; }

    bool next() {
        _error = Error(f_readdir(&_dir, &_info._info));
        return _error == OK && _info.name()[0] != '\0';
    }

private:
    DIR _dir;
    Error _error = OK;
    FileInfo _info;
};

} // namespace fs
