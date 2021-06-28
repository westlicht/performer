#include "FileManager.h"
#include "ProjectVersion.h"

#include "core/utils/StringBuilder.h"
#include "core/fs/FileSystem.h"
#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

#include "os/os.h"

#include <algorithm>

#include <cstring>

uint32_t FileManager::_volumeState = 0;
uint32_t FileManager::_nextVolumeStateCheckTicks = 0;

std::array<FileManager::CachedSlotInfo, 4> FileManager::_cachedSlotInfos;
uint32_t FileManager::_cachedSlotInfoTicket = 0;

FileManager::TaskExecuteCallback FileManager::_taskExecuteCallback;
FileManager::TaskResultCallback FileManager::_taskResultCallback;
volatile uint32_t FileManager::_taskPending;

struct FileTypeInfo {
    const char *dir;
    const char *ext;
};

FileTypeInfo fileTypeInfos[] = {
    { "PROJECTS", "PRO" },
    { "SCALES", "SCA" },
};

static void slotPath(StringBuilder &str, FileType type, int slot) {
    const auto &info = fileTypeInfos[int(type)];
    str("%s/%03d.%s", info.dir, slot + 1, info.ext);
}

void FileManager::init() {
    _volumeState = 0;
    _nextVolumeStateCheckTicks = 0;
    _taskExecuteCallback = nullptr;
    _taskResultCallback = nullptr;
    _taskPending = 0;
}

bool FileManager::volumeAvailable() {
    return _volumeState & Available;
}

bool FileManager::volumeMounted() {
    return _volumeState & Mounted;
}

fs::Error FileManager::format() {
    invalidateAllSlots();
    return fs::volume().format();
}

fs::Error FileManager::writeProject(Project &project, int slot) {
    return writeFile(FileType::Project, slot, [&] (const char *path) {
        auto result = writeProject(project, path);
        if (result == fs::OK) {
            project.setSlot(slot);
            writeLastProject(slot);
        }
        return result;
    });
}

fs::Error FileManager::readProject(Project &project, int slot) {
    return readFile(FileType::Project, slot, [&] (const char *path) {
        auto result = readProject(project, path);
        if (result == fs::OK) {
            project.setSlot(slot);
            writeLastProject(slot);
        }
        return result;
    });
}

fs::Error FileManager::readLastProject(Project &project) {
    int slot;

    auto result = readLastProject(slot);

    if (result == fs::OK && slot >= 0) {
        result = readProject(project, slot);
        project.setAutoLoaded(true);
    }

    return result;
}

fs::Error FileManager::writeUserScale(const UserScale &userScale, int slot) {
    return writeFile(FileType::UserScale, slot, [&] (const char *path) {
        return writeUserScale(userScale, path);
    });
}

fs::Error FileManager::readUserScale(UserScale &userScale, int slot) {
    return readFile(FileType::UserScale, slot, [&] (const char *path) {
        return readUserScale(userScale, path);
    });
}

fs::Error FileManager::writeProject(const Project &project, const char *path) {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        return fileWriter.error();
    }

    FileHeader header(FileType::Project, 0, project.name());
    fileWriter.write(&header, sizeof(header));

    VersionedSerializedWriter writer(
        [&fileWriter] (const void *data, size_t len) { fileWriter.write(data, len); },
        ProjectVersion::Latest
    );

    project.write(writer);

    return fileWriter.finish();
}

fs::Error FileManager::readProject(Project &project, const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        return fileReader.error();
    }

    FileHeader header;
    fileReader.read(&header, sizeof(header));

    VersionedSerializedReader reader(
        [&fileReader] (void *data, size_t len) { fileReader.read(data, len); },
        ProjectVersion::Latest
    );

    bool success = project.read(reader);

    auto error = fileReader.finish();
    if (error == fs::OK && !success) {
        error = fs::INVALID_CHECKSUM;
    }

    return error;
}

fs::Error FileManager::writeUserScale(const UserScale &userScale, const char *path) {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        return fileWriter.error();
    }

    FileHeader header(FileType::UserScale, 0, userScale.name());
    fileWriter.write(&header, sizeof(header));

    VersionedSerializedWriter writer(
        [&fileWriter] (const void *data, size_t len) { fileWriter.write(data, len); },
        ProjectVersion::Latest
    );

    userScale.write(writer);

    return fileWriter.finish();
}

fs::Error FileManager::readUserScale(UserScale &userScale, const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        return fileReader.error();
    }

    FileHeader header;
    fileReader.read(&header, sizeof(header));

    VersionedSerializedReader reader(
        [&fileReader] (void *data, size_t len) { fileReader.read(data, len); },
        ProjectVersion::Latest
    );

    bool success = userScale.read(reader);

    auto error = fileReader.finish();
    if (error == fs::OK && !success) {
        error = fs::INVALID_CHECKSUM;
    }

    return error;
}

fs::Error FileManager::writeSettings(const Settings &settings, const char *path) {
    fs::FileWriter fileWriter(path);
    if (fileWriter.error() != fs::OK) {
        return fileWriter.error();
    }

    FileHeader header(FileType::Settings, 0, "SETTINGS");
    fileWriter.write(&header, sizeof(header));

    VersionedSerializedWriter writer(
        [&fileWriter] (const void *data, size_t len) { fileWriter.write(data, len); },
        Settings::Version
    );

    settings.write(writer);

    return fileWriter.finish();
}

fs::Error FileManager::readSettings(Settings &settings, const char *path) {
    fs::FileReader fileReader(path);
    if (fileReader.error() != fs::OK) {
        return fileReader.error();
    }

    FileHeader header;
    fileReader.read(&header, sizeof(header));

    VersionedSerializedReader reader(
        [&fileReader] (void *data, size_t len) { fileReader.read(data, len); },
        Settings::Version
    );

    bool success = settings.read(reader);

    auto error = fileReader.finish();
    if (error == fs::OK && !success) {
        error = fs::INVALID_CHECKSUM;
    }

    return error;
}

void FileManager::slotInfo(FileType type, int slot, SlotInfo &info) {
    if (cachedSlot(type, slot, info)) {
        return;
    }

    info.used = false;

    FixedStringBuilder<32> path;
    slotPath(path, type, slot);

    if (fs::exists(path)) {
        fs::File file(path, fs::File::Read);
        FileHeader header;
        size_t lenRead;
        if (file.read(&header, sizeof(header), &lenRead) == fs::OK && lenRead == sizeof(header)) {
            header.readName(info.name, sizeof(info.name));
            info.used = true;
        }
    }

    cacheSlot(type, slot, info);
}

bool FileManager::slotUsed(FileType type, int slot) {
    SlotInfo info;
    slotInfo(type, slot, info);
    return info.used;
}

void FileManager::task(TaskExecuteCallback executeCallback, TaskResultCallback resultCallback) {
    _taskExecuteCallback = executeCallback;
    _taskResultCallback = resultCallback;
    _taskPending = 1;
}

void FileManager::processTask() {
    // check volume availability & mount
    uint32_t ticks = os::ticks();
    if (ticks >= _nextVolumeStateCheckTicks) {
        _nextVolumeStateCheckTicks = ticks + os::time::ms(1000);

        uint32_t newVolumeState = fs::volume().available() ? Available : 0;
        if (newVolumeState & Available) {
            if (!(_volumeState & Mounted)) {
                newVolumeState |= (fs::volume().mount() == fs::OK) ? Mounted : 0;
            } else {
                newVolumeState |= Mounted;
            }
        } else {
            invalidateAllSlots();
        }

        _volumeState = newVolumeState;
    }

    if (_taskPending) {
        fs::Error result = _taskExecuteCallback();
        _taskPending = 0;
        _taskResultCallback(result);
    }
}


fs::Error FileManager::writeFile(FileType type, int slot, std::function<fs::Error(const char *)> write) {
    const auto &info = fileTypeInfos[int(type)];
    if (!fs::exists(info.dir)) {
        fs::mkdir(info.dir);
    }

    FixedStringBuilder<32> path;
    slotPath(path, type, slot);

    auto result = write(path);
    if (result == fs::OK) {
        invalidateSlot(type, slot);
    }

    return result;
}

fs::Error FileManager::readFile(FileType type, int slot, std::function<fs::Error(const char *)> read) {
    const auto &info = fileTypeInfos[int(type)];
    if (!fs::exists(info.dir)) {
        fs::mkdir(info.dir);
    }

    FixedStringBuilder<32> path;
    slotPath(path, type, slot);

    auto result = read(path);

    return result;
}

fs::Error FileManager::writeLastProject(int slot) {
    fs::FileWriter fileWriter("LAST.DAT");
    if (fileWriter.error() != fs::OK) {
        return fileWriter.error();
    }

    fileWriter.write(&slot, sizeof(slot));

    return fileWriter.finish();
}

fs::Error FileManager::readLastProject(int &slot) {
    fs::FileReader fileReader("LAST.DAT");
    if (fileReader.error() != fs::OK) {
        return fileReader.error();
    }

    fileReader.read(&slot, sizeof(slot));

    return fileReader.finish();
}

bool FileManager::cachedSlot(FileType type, int slot, SlotInfo &info) {
    for (auto &cachedSlotInfo : _cachedSlotInfos) {
        if (cachedSlotInfo.ticket != 0 && cachedSlotInfo.type == type && cachedSlotInfo.slot == slot) {
            info = cachedSlotInfo.info;
            cachedSlotInfo.ticket = nextCachedSlotTicket();
            return true;
        }
    }
    return false;
}

void FileManager::cacheSlot(FileType type, int slot, const SlotInfo &info) {
    auto cachedSlotInfo = std::min_element(_cachedSlotInfos.begin(), _cachedSlotInfos.end());
    cachedSlotInfo->type = type;
    cachedSlotInfo->slot = slot;
    cachedSlotInfo->info = info;
    cachedSlotInfo->ticket = nextCachedSlotTicket();
}

void FileManager::invalidateSlot(FileType type, int slot) {
    for (auto &cachedSlotInfo : _cachedSlotInfos) {
        if (cachedSlotInfo.ticket != 0 && cachedSlotInfo.type == type && cachedSlotInfo.slot == slot) {
            cachedSlotInfo.ticket = 0;
        }
    }
}

void FileManager::invalidateAllSlots() {
    for (auto &cachedSlotInfo : _cachedSlotInfos) {
        cachedSlotInfo.ticket = 0;
    }
}

uint32_t FileManager::nextCachedSlotTicket() {
    _cachedSlotInfoTicket = std::max(uint32_t(1), _cachedSlotInfoTicket + 1);
    return _cachedSlotInfoTicket;
}
