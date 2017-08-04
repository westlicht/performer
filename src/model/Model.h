#pragma once

#include "Project.h"
#include "Settings.h"

class Model {
public:
    Model() {
    }

    const Project &project() const { return _project; }
          Project &project()       { return _project; }

    const Settings &settings() const { return _settings; }
          Settings &settings()       { return _settings; }

private:
    Project _project;
    Settings _settings;
};
