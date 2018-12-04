#include "FileSystem.h"

#include "ff/ff.h"
#include "ff/diskio.h"

#include "os/os.h"


namespace fs {

static Volume *g_volume;
static SdCard *g_sdCard;

void setVolume(Volume *volume) {
    ASSERT(volume == nullptr || g_volume == nullptr, "only one volume allowed");
    g_volume = volume;
    g_sdCard = volume ? &volume->sdcard() : nullptr;
}

Volume &volume() {
    return *g_volume;
}

Error mkdir(const char *path) {
    return Error(f_mkdir(path));
}

Error rmdir(const char *path) {
    return Error(f_unlink(path));
}

Error remove(const char *path) {
    return Error(f_unlink(path));
}

Error rename(const char *oldPath, const char *newPath) {
    return Error(f_rename(oldPath, newPath));
}

Error stat(const char *path, FileInfo &info) {
    return Error(f_stat(path, &info._info));
}

bool exists(const char *path) {
    FileInfo info;
    return stat(path, info) == OK;
}

} // namespace fs


extern "C" {

DSTATUS disk_initialize(BYTE pdrv) {
    ASSERT(pdrv == 0, "only one physical drive available");
    // DBG("disk_initialize(pdrv=%d)", pdrv);
    return fs::g_sdCard->available() ? 0 : STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) {
    ASSERT(pdrv == 0, "only one physical drive available");
    // DBG("disk_status(pdrv=%d)", pdrv);
    if (!fs::g_sdCard->available()) {
        return STA_NODISK;
    }
    if (fs::g_sdCard->writeProtected()) {
        return STA_PROTECT;
    }
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buf, DWORD sector, UINT count) {
    ASSERT(pdrv == 0, "only one physical drive available");
    // DBG("disk_read(pdrv=%d,sector=%d,count=%d)", pdrv, sector, count);
    return fs::g_sdCard->read(buf, sector, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buf, DWORD sector, UINT count) {
    ASSERT(pdrv == 0, "only one physical drive available");
    // DBG("disk_write(pdrv=%d,sector=%d,count=%d)", pdrv, sector, count);
    return fs::g_sdCard->write(buf, sector, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buf) {
    ASSERT(pdrv == 0, "only one physical drive available");
    // DBG("disk_ioctl(pdrv=%d,cmd=%d)", pdrv, cmd);
    switch (cmd) {
    case CTRL_SYNC:
        fs::g_sdCard->sync();
        return RES_OK;
    case GET_SECTOR_COUNT:
        *static_cast<DWORD *>(buf) = fs::g_sdCard->sectorCount();
        return RES_OK;
    case GET_SECTOR_SIZE:
        *static_cast<WORD *>(buf) = fs::g_sdCard->sectorSize();
        return RES_OK;
    case GET_BLOCK_SIZE:
        *static_cast<DWORD *>(buf) = 1;
        return RES_OK;
    }
	return RES_PARERR;
}

#if !_FS_READONLY
DWORD get_fattime() {
    return 0;
}
#endif

#if _FS_REENTRANT

static os::Mutex s_mutex[_VOLUMES];

int ff_cre_syncobj(BYTE vol, _SYNC_t *sobj) {
    *sobj = vol;
    return 1;
}

int ff_del_syncobj(_SYNC_t sobj) {
    return 1;
}

int ff_req_grant(_SYNC_t sobj) {
	return s_mutex[sobj].take(_FS_TIMEOUT);
}

void ff_rel_grant(_SYNC_t sobj) {
    s_mutex[sobj].give();
}

#endif // _FS_REENTRANT

} // extern "C"
