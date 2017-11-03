#pragma once

#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"
#include "core/io/SerializedWriter.h"
#include "core/io/SerializedReader.h"

typedef SerializedWriter<fs::FileWriter> ModelWriter;
typedef SerializedReader<fs::FileReader> ModelReader;
