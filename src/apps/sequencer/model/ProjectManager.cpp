#include "ProjectManager.h"

#include "core/utils/StringBuilder.h"

#include <algorithm>

std::array<ProjectManager::CachedSlotInfo, 4> ProjectManager::_cachedSlotInfos;
uint32_t ProjectManager::_cachedSlotInfoTicket = 0;

static void projectFilename(StringBuilder &str, int slot) {
    str("%03d.pro", slot + 1);
}

fs::Error ProjectManager::format() {
    invalidateAllSlots();
    return fs::volume().format();
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
        invalidateSlot(slot);
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
    if (cachedSlot(slot, info)) {
        return;
    }

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

    cacheSlot(slot, info);
}

bool ProjectManager::cachedSlot(int slot, SlotInfo &info) {
    for (auto &cachedSlotInfo : _cachedSlotInfos) {
        if (cachedSlotInfo.ticket != 0 && cachedSlotInfo.slot == slot) {
            info = cachedSlotInfo.info;
            cachedSlotInfo.ticket = nextCachedSlotTicket();
            return true;
        }
    }
    return false;
}

void ProjectManager::cacheSlot(int slot, const SlotInfo &info) {
    auto cachedSlotInfo = std::min_element(_cachedSlotInfos.begin(), _cachedSlotInfos.end());
    cachedSlotInfo->slot = slot;
    cachedSlotInfo->info = info;
    cachedSlotInfo->ticket = nextCachedSlotTicket();
}

void ProjectManager::invalidateSlot(int slot) {
    for (auto &cachedSlotInfo : _cachedSlotInfos) {
        if (cachedSlotInfo.ticket != 0 && cachedSlotInfo.slot == slot) {
            cachedSlotInfo.ticket = 0;
        }
    }
}

void ProjectManager::invalidateAllSlots() {
    for (auto &cachedSlotInfo : _cachedSlotInfos) {
        cachedSlotInfo.ticket = 0;
    }
}

uint32_t ProjectManager::nextCachedSlotTicket() {
    _cachedSlotInfoTicket = std::max(uint32_t(1), _cachedSlotInfoTicket + 1);
    return _cachedSlotInfoTicket;
}

