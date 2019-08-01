#pragma once

#include "core/fs/FileSystem.h"
#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"
#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"

#include <array>

#include <cstdlib>
#include <cstdint>

struct WriteContext {
    VersionedSerializedWriter &writer;
};

struct ReadContext {
    VersionedSerializedReader &reader;
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
static void readArray(ReadContext &context, std::array<T, N> &array, size_t size = N) {
    for (size_t i = 0; i < size; ++i) {
        array[i].read(context);
    }
}

template<size_t N>
static void readArray(ReadContext &context, std::array<uint8_t, N> &array, size_t size = N) {
    for (size_t i = 0; i < size; ++i) {
        context.reader.read(array[i]);
    }
}
