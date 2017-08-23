#include "Volume.h"
#include "FileSystem.h"

namespace fs {

Volume::Volume(SDCard &sdcard) :
    _sdcard(sdcard)
{
    setVolume(*this);
}

bool Volume::available() {
    return _sdcard.available();
}

Error Volume::format() {
    uint8_t workArea[512];
    return Error(f_mkfs("", FM_FAT, 0, workArea, sizeof(workArea)));
}

Error Volume::mount() {
    return Error(f_mount(&_fs, "", 1));
}

Error Volume::unmount() {
    return Error(f_unmount(""));
}

Error Volume::stats(size_t *total, size_t *free) const {
    FATFS *fs;
    DWORD freeClusters;
    Error result = Error(f_getfree("", &freeClusters, &fs));
    if (result != OK) {
        return result;
    }

    if (total) {
        *total = ((fs->n_fatent - 2) * fs->csize) / 2;
    }
    if (free) {
        *free = (freeClusters * fs->csize) / 2;
    }

    return result;
}

size_t Volume::sizeTotal() const {
    size_t size = 0;
    stats(&size, nullptr);
    return size;
}

size_t Volume::sizeFree() const {
    size_t size = 0;
    stats(nullptr, &size);
    return size;
}

} // namespace fs
