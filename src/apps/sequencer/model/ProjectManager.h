#pragma once

#include "Project.h"

#include "core/fs/FileSystem.h"

#include <array>

class ProjectManager {
public:
    static fs::Error format();

    static fs::Error saveProject(Project &project, int slot);
    static fs::Error loadProject(Project &project, int slot);

    struct SlotInfo {
        bool used;
        char name[Project::NameLength + 1];
    };

    static void slotInfo(int slot, SlotInfo &info);

private:
    static bool cachedSlot(int slot, SlotInfo &info);
    static void cacheSlot(int slot, const SlotInfo &info);
    static void invalidateSlot(int slot);
    static void invalidateAllSlots();
    static uint32_t nextCachedSlotTicket();

    struct CachedSlotInfo {
        uint32_t ticket = 0;
        uint8_t slot;
        SlotInfo info;

        bool operator < (const CachedSlotInfo &other) const {
            return ticket < other.ticket;
        }
    };

    static std::array<CachedSlotInfo, 4> _cachedSlotInfos;
    static uint32_t _cachedSlotInfoTicket;
};
