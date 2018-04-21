#pragma once

#include <algorithm>

#include <cstring>
#include <cstdint>

enum class FileType : uint8_t {
    Project     = 0,
    UserScale   = 1,
    Last
};

struct FileHeader {
    static const size_t NameLength = 8;

    FileType type;
    uint8_t version;
    char name[NameLength];

    FileHeader() = default;

    FileHeader(FileType type, uint8_t version, const char *name) {
        this->type = type;
        this->version = version;
        size_t len = strlen(name);
        std::memset(this->name, 0, NameLength);
        std::memcpy(this->name, name, std::min(NameLength, len));
    }

    void readName(char *name, size_t len) {
        std::memcpy(name, this->name, std::min(NameLength, len));
        name[std::min(NameLength, len - 1)] = '\0';
    }

} __attribute__((packed));

