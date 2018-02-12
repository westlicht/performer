#include "Model.h"

#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

fs::Error Model::write(const char *path) {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        fileWriter.error();
    }

    ModelWriter modelWriter(fileWriter);
    WriteContext context = { *this, modelWriter };

    write(context);

    return fileWriter.finish();
}

fs::Error Model::read(const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        fileReader.error();
    }

    ModelReader modelReader(fileReader);
    ReadContext context = { *this, modelReader };

    read(context);

    return fileReader.finish();
}

void Model::write(WriteContext &context) const {
    _project.write(context);
    _settings.write(context);
}

void Model::read(ReadContext &context) {
    _project.read(context);
    _settings.read(context);
}
