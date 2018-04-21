#include "File.h"

#include "core/utils/ObjectPool.h"

#include "os/os.h"
#include "os/LockGuard.h"

namespace fs {

static ObjectPool<FIL, 2> filePool;
static os::Mutex filePoolMutex;

FIL *File::allocateFile() {
    os::LockGuard lock(filePoolMutex);
    FIL *file = filePool.allocate();
    ASSERT(file, "no free files");
    return file;
}

void File::releaseFile(FIL *file) {
    os::LockGuard lock(filePoolMutex);
    filePool.release(file);
}

} // namespace fs
