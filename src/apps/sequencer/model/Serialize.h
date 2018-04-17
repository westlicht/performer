#pragma once

#include "core/fs/FileSystem.h"
#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"
#include "core/io/SerializedWriter.h"
#include "core/io/SerializedReader.h"

#include <array>

#include <cstdlib>
#include <cstdint>

typedef SerializedWriter<fs::FileWriter> Writer;
typedef SerializedReader<fs::FileReader> Reader;

struct WriteContext {
    Writer &writer;
};

struct ReadContext {
    Reader &reader;
};

enum class FileType : uint8_t {
    Project     = 0,
    UserScale   = 1,
};

struct FileHeader {
    FileType type;
    char name[9];
};

template<typename T, size_t N>
static void writeArray(WriteContext &context, const std::array<T, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        array[i].write(context);
    }
}

template<size_t N>
static void writeArray(WriteContext &context, const std::array<uint8_t, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        context.writer.write(array[i]);
    }
}

template<typename T, size_t N>
static void readArray(ReadContext &context, std::array<T, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        array[i].read(context);
    }
}

template<size_t N>
static void readArray(ReadContext &context, std::array<uint8_t, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        context.reader.read(array[i]);
    }
}
