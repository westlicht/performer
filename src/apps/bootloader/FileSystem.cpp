#include "FileSystem.h"
#include "SdCard.h"
#include "Debug.h"

#include "lib/ff/diskio.h"

extern "C" {

DSTATUS disk_initialize(BYTE pdrv) {
    DBG("disk_initialize(pdrv=%d)", pdrv);
    return SdCard::available() ? 0 : STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv) {
    // ASSERT(pdrv == 0, "only one physical drive available");
    // DBG("disk_status(pdrv=%d)", pdrv);
    if (!SdCard::available()) {
        return STA_NODISK;
    }
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buf, DWORD sector, UINT count) {
    // ASSERT(pdrv == 0, "only one physical drive available");
    // DBG("disk_read(pdrv=%d,sector=%d,count=%d)", pdrv, sector, count);
    return SdCard::read(buf, sector, count) ? RES_OK : RES_ERROR;
}

// DRESULT disk_write(BYTE pdrv, const BYTE *buf, DWORD sector, UINT count) {
//     ASSERT(pdrv == 0, "only one physical drive available");
//     // DBG("disk_write(pdrv=%d,sector=%d,count=%d)", pdrv, sector, count);
//     return fs::g_sdCard->write(buf, sector, count) ? RES_OK : RES_ERROR;
// }

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buf) {
    // ASSERT(pdrv == 0, "only one physical drive available");
    DBG("disk_ioctl(pdrv=%d,cmd=%d)", pdrv, cmd);
    // switch (cmd) {
    // case CTRL_SYNC:
    //     fs::g_sdCard->sync();
    //     return RES_OK;
    // case GET_SECTOR_COUNT:
    //     *static_cast<DWORD *>(buf) = fs::g_sdCard->sectorCount();
    //     return RES_OK;
    // case GET_SECTOR_SIZE:
    //     *static_cast<WORD *>(buf) = fs::g_sdCard->sectorSize();
    //     return RES_OK;
    // case GET_BLOCK_SIZE:
    //     *static_cast<DWORD *>(buf) = 1;
    //     return RES_OK;
    // }
	return RES_PARERR;
}

} // extern "C"
