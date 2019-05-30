#pragma once

#include <algorithm>

#include <cstring>
#include <cstdint>

enum class FileType : uint8_t {
    Project     = 0,
    UserScale   = 1,
    Settings    = 255
};

struct FileHeader {
    static constexpr size_t NameLength = 8;

    FileType type;
    uint8_t version;
    char name[NameLength];

    FileHeader() = default;

    FileHeader(FileType type_, uint8_t version_, const char *name_) {
        type = type_;
        version = version_;
        size_t len = strlen(name_);
        std::memset(name, 0, sizeof(name));
        std::memcpy(name, name_, std::min(sizeof(name), len));
    }

    void readName(char *name_, size_t len) {
        std::memcpy(name_, name, std::min(sizeof(name), len));
        name_[std::min(sizeof(name), len - 1)] = '\0';
    }

} __attribute__((packed));

