#pragma once

#include "core/io/VersionedSerializedWriter.h"
#include "core/io/VersionedSerializedReader.h"

#include <array>

#include <cstdlib>
#include <cstdint>

template<typename T, size_t N>
static void writeArray(VersionedSerializedWriter &writer, const std::array<T, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        array[i].write(writer);
    }
}

template<size_t N>
static void writeArray(VersionedSerializedWriter &writer, const std::array<uint8_t, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        writer.write(array[i]);
    }
}

template<typename T, size_t N>
static void readArray(VersionedSerializedReader &reader, std::array<T, N> &array, size_t size = N) {
    for (size_t i = 0; i < size; ++i) {
        array[i].read(reader);
    }
}

template<size_t N>
static void readArray(VersionedSerializedReader &reader, std::array<uint8_t, N> &array, size_t size = N) {
    for (size_t i = 0; i < size; ++i) {
        reader.read(array[i]);
    }
}
