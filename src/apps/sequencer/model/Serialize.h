#pragma once

#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"
#include "core/io/SerializedWriter.h"
#include "core/io/SerializedReader.h"

#include <array>

#include <cstdlib>

typedef SerializedWriter<fs::FileWriter> ModelWriter;
typedef SerializedReader<fs::FileReader> ModelReader;

class Model;

struct WriteContext {
    Model &model;
    ModelWriter &writer;
};

struct ReadContext {
    Model &model;
    ModelReader &reader;
};

template<typename T, size_t N>
static void writeArray(WriteContext &context, const std::array<T, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        array[i].write(context, i);
    }
}

template<typename T, size_t N>
static void readArray(ReadContext &context, std::array<T, N> &array) {
    for (size_t i = 0; i < array.size(); ++i) {
        array[i].read(context, i);
    }
}
