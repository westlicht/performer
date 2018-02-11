#pragma once

#include "Project.h"
#include "Settings.h"
#include "Serialize.h"

class Model {
public:
    Model() {
    }

    const Project &project() const { return _project; }
          Project &project()       { return _project; }

    const Settings &settings() const { return _settings; }
          Settings &settings()       { return _settings; }

    // Serialization

    bool write(const char *path);
    bool read(const char *path);

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    Project _project;
    Settings _settings;
};
