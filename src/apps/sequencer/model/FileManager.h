#pragma once

#include "FileDefs.h"
#include "Project.h"
#include "UserScale.h"
#include "Settings.h"

#include "core/fs/FileSystem.h"

#include <array>
#include <functional>

#include <cstdint>

class FileManager {
public:
    static void init();

    static bool volumeAvailable();
    static bool volumeMounted();

    static fs::Error format();

    static fs::Error writeProject(Project &project, int slot);
    static fs::Error readProject(Project &project, int slot);
    static fs::Error readLastProject(Project &project);

    static fs::Error writeUserScale(const UserScale &userScale, int slot);
    static fs::Error readUserScale(UserScale &userScale, int slot);

    static fs::Error writeProject(const Project &project, const char *path);
    static fs::Error readProject(Project &project, const char *path);

    static fs::Error writeUserScale(const UserScale &userScale, const char *path);
    static fs::Error readUserScale(UserScale &userScale, const char *path);

    static fs::Error writeSettings(const Settings &settings, const char *path);
    static fs::Error readSettings(Settings &settings, const char *path);

    // Slot information

    struct SlotInfo {
        bool used;
        char name[FileHeader::NameLength + 1];
    };

    static void slotInfo(FileType type, int slot, SlotInfo &info);
    static bool slotUsed(FileType type, int slot);

    // File tasks

    typedef std::function<fs::Error(void)> TaskExecuteCallback;
    typedef std::function<void(fs::Error)> TaskResultCallback;

    static void task(TaskExecuteCallback executeCallback, TaskResultCallback resultCallback);
    static void processTask();

private:
    static fs::Error writeFile(FileType type, int slot, std::function<fs::Error(const char *)> write);
    static fs::Error readFile(FileType type, int slot, std::function<fs::Error(const char *)> read);

    static fs::Error writeLastProject(int slot);
    static fs::Error readLastProject(int &slot);

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

    enum VolumeState {
        Available   = (1<<0),
        Mounted     = (1<<1),
    };

    static uint32_t _volumeState;
    static uint32_t _nextVolumeStateCheckTicks;

    static std::array<CachedSlotInfo, 4> _cachedSlotInfos;
    static uint32_t _cachedSlotInfoTicket;

    static TaskExecuteCallback _taskExecuteCallback;
    static TaskResultCallback _taskResultCallback;
    static volatile uint32_t _taskPending;
};
