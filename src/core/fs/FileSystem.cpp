#include "FileSystem.h"

#include "ff/ff.h"
#include "ff/diskio.h"

#include "os/os.h"

static SDCard *g_sdcard;

namespace fs {

static Volume *g_volume;

void setVolume(Volume &volume) {
    ASSERT(g_volume == nullptr, "only one volume allowed");
    g_volume = &volume;
    g_sdcard = &volume.sdcard();
}

Volume &volume() {
    return *g_volume;
}

} // namespace fs


extern "C" {

DSTATUS disk_initialize(BYTE pdrv) {
    ASSERT(pdrv == 0, "only one physical drive available");
    g_sdcard->init();
    return 0;
}

DSTATUS disk_status(BYTE pdrv) {
    ASSERT(pdrv == 0, "only one physical drive available");
    if (!g_sdcard->available()) {
        return STA_NODISK;
    }
    if (g_sdcard->writeProtected()) {
        return STA_PROTECT;
    }
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buf, DWORD sector, UINT count) {
    ASSERT(pdrv == 0, "only one physical drive available");
    return g_sdcard->read(buf, sector, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buf, DWORD sector, UINT count) {
    ASSERT(pdrv == 0, "only one physical drive available");
    return g_sdcard->write(buf, sector, count) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buf) {
    ASSERT(pdrv == 0, "only one physical drive available");
    switch (cmd) {
    case CTRL_SYNC:
        g_sdcard->sync();
        return RES_OK;
    case GET_SECTOR_COUNT:
        *static_cast<DWORD *>(buf) = g_sdcard->sectorCount();
        return RES_OK;
    case GET_SECTOR_SIZE:
        *static_cast<WORD *>(buf) = g_sdcard->sectorSize();
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
