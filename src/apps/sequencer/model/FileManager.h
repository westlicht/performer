#pragma once

#include "FileDefs.h"
#include "Project.h"
#include "UserScale.h"

#include "core/fs/FileSystem.h"

#include <array>
#include <functional>

#include <cstdint>

class FileManager {
public:
    static fs::Error format();

    static fs::Error saveProject(Project &project, int slot);
    static fs::Error loadProject(Project &project, int slot);

    static fs::Error saveUserScale(UserScale &userScale, int slot);
    static fs::Error loadUserScale(UserScale &userScale, int slot);

    struct SlotInfo {
        bool used;
        char name[FileHeader::NameLength + 1];
    };

    static void slotInfo(FileType type, int slot, SlotInfo &info);
    static bool slotUsed(FileType type, int slot);

private:
    static fs::Error saveFile(FileType type, int slot, std::function<fs::Error(const char *)> write);
    static fs::Error loadFile(FileType type, int slot, std::function<fs::Error(const char *)> read);

    static bool cachedSlot(FileType type, int slot, SlotInfo &info);
    static void cacheSlot(FileType type, int slot, const SlotInfo &info);
    static void invalidateSlot(FileType type, int slot);
    static void invalidateAllSlots();
    static uint32_t nextCachedSlotTicket();

    struct CachedSlotInfo {
        uint32_t ticket = 0;
        FileType type;
        uint8_t slot;
        SlotInfo info;

        bool operator < (const CachedSlotInfo &other) const {
            return ticket < other.ticket;
        }
    };

    static std::array<CachedSlotInfo, 4> _cachedSlotInfos;
    static uint32_t _cachedSlotInfoTicket;
};
