#pragma once

#include "Project.h"

#include "core/fs/FileSystem.h"

class ProjectManager {
public:
    static fs::Error saveProject(Project &project, int slot);
    static fs::Error loadProject(Project &project, int slot);

    struct SlotInfo {
        bool used;
        char name[Project::NameLength + 1];
    };

    static void slotInfo(int slot, SlotInfo &info);
};
