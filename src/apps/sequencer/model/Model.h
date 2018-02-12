#pragma once

#include "Project.h"
#include "Settings.h"
#include "Serialize.h"

#include "core/fs/FileSystem.h"

class Model {
public:
    Model() {
    }

    const Project &project() const { return _project; }
          Project &project()       { return _project; }

    const Settings &settings() const { return _settings; }
          Settings &settings()       { return _settings; }

    // Serialization

    fs::Error write(const char *path);
    fs::Error read(const char *path);

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Project _project;
    Settings _settings;
};
