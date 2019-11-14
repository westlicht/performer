#pragma once

#include "Project.h"
#include "Settings.h"
#include "ClipBoard.h"
#include "Serialize.h"

#include "os/os.h"

class Model {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    class WriteLock : public os::InterruptLock {};

    class ConfigLock {
    public:
        ConfigLock() {
        }

        ~ConfigLock() {
        }
    };

    //----------------------------------------
    // Properties
    //----------------------------------------

    const Project &project() const { return _project; }
          Project &project()       { return _project; }

    const Settings &settings() const { return _settings; }
          Settings &settings()       { return _settings; }

    const ClipBoard &clipBoard() const { return _clipBoard; }
          ClipBoard &clipBoard()       { return _clipBoard; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    Model();

    void init();

private:
    Project _project;
    Settings _settings;
    ClipBoard _clipBoard;
};
