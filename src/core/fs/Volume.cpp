#include "Volume.h"
#include "FileSystem.h"

extern "C" {
    PARTITION VolToPart[] = {
        {0, 1} // "0:" ==> Physical drive 0, 1st partition
    };
}

namespace fs {

Volume::Volume(SdCard &sdcard) :
    _sdcard(sdcard)
{
    setVolume(this);
}

Volume::~Volume() {
    unmount();
    setVolume(nullptr);
}

bool Volume::available() {
    return _sdcard.available();
}

Error Volume::format() {
    // TODO we might want to reuse file object pool memory for this
    uint32_t workArea[FF_MAX_SS / 4];

    DWORD plist[] = { 100, 0, 0, 0 };
    Error result = Error(f_fdisk(0, plist, workArea));
    if (result != OK) {
        return result;
    }
    return Error(f_mkfs("", FM_ANY, 0, workArea, sizeof(workArea)));
}

Error Volume::mount() {
    return Error(f_mount(&_fs, "", 1));
}

Error Volume::unmount() {
    return Error(f_mount(nullptr, "", 0));
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
