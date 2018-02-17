#include "ProjectManager.h"

#include "core/utils/StringBuilder.h"

static void projectFilename(StringBuilder &str, int slot) {
    str("%03d.pro", slot + 1);
}

fs::Error ProjectManager::saveProject(Project &project, int slot) {
    auto result = fs::volume().mount();
    if (result != fs::OK) {
        return result;
    }

    FixedStringBuilder<16> filename;
    projectFilename(filename, slot);

    result = project.write(filename);
    if (result == fs::OK) {
        project.setSlot(slot);
    }

    return result;
}

fs::Error ProjectManager::loadProject(Project &project, int slot) {
    auto result = fs::volume().mount();
    if (result != fs::OK) {
        return result;
    }

    FixedStringBuilder<16> filename;
    projectFilename(filename, slot);

    result = project.read(filename);
    if (result == fs::OK) {
        project.setSlot(slot);
    }

    return result;
}

void ProjectManager::slotInfo(int slot, SlotInfo &info) {
    info.used = false;

    if (fs::volume().mount() != fs::OK) {
        return;
    }

    FixedStringBuilder<16> filename;
    projectFilename(filename, slot);

    if (fs::exists(filename)) {
        fs::File file(filename, fs::File::Read);
        size_t lenRead;
        if (file.read(info.name, Project::NameLength + 1, &lenRead) == fs::OK && lenRead == Project::NameLength + 1) {
            info.used = true;
        }
    }
}
