#include "UpdateFile.h"
#include "FileSystem.h"
#include "Console.h"

static FATFS fs;
static FIL fil;

bool UpdateFile::open(VersionTag &version, size_t &size, uint8_t md5[16], char *errorStr, size_t errorLen) {
    FRESULT result;

    result = f_mount(&fs, "", 1);
    if (result != FR_OK) {
        snprintf(errorStr, errorLen, "failed to mount (error: %d)", result);
        return false;
    }

    result = f_open(&fil, CONFIG_UPDATE_FILENAME, FA_READ);
    if (result != FR_OK) {
        snprintf(errorStr, errorLen, "failed to open file (error: %d)", result);
        return false;
    }

    FILINFO info;
    result = f_stat(CONFIG_UPDATE_FILENAME, &info);
    if (result != FR_OK) {
        snprintf(errorStr, errorLen, "failed to stat file (error: %d)", result);
        return false;
    }

    size = info.fsize - 16; // md5 in hex at the end of the file

    result = f_lseek(&fil, CONFIG_VERSION_TAG_OFFSET);
    if (result != FR_OK) {
        snprintf(errorStr, errorLen, "failed to seek version (result: %d)", result);
        return false;
    }

    size_t bytesRead;
    result = f_read(&fil, &version, sizeof(version), &bytesRead);
    if (result != FR_OK || bytesRead != sizeof(VersionTag)) {
        snprintf(errorStr, errorLen, "failed to read version tag (error: %d)", result);
        return false;
    }

    result = f_lseek(&fil, size);
    if (result != FR_OK) {
        snprintf(errorStr, errorLen, "failed to seek checksum (result: %d)", result);
        return false;
    }

    result = f_read(&fil, md5, 16, &bytesRead);
    if (result != FR_OK || bytesRead != 16) {
        snprintf(errorStr, errorLen, "failed to read checksum (error: %d)", result);
        return false;
    }

    return rewind(errorStr, errorLen);
}

bool UpdateFile::rewind(char *errorStr, size_t errorLen) {
    FRESULT result = f_lseek(&fil, 0);
    if (result != FR_OK) {
        snprintf(errorStr, errorLen, "failed to seek start (result: %d)", result);
        return false;
    }

    return true;
}

bool UpdateFile::read(void *readBuf, size_t readLen, char *errorStr, size_t errorLen) {
    uint8_t *readPos = reinterpret_cast<uint8_t *>(readBuf);
    while (readLen > 0) {
        size_t bytesRead;
        FRESULT result = f_read(&fil, readPos, readLen, &bytesRead);
        if (result != FR_OK || bytesRead == 0) {
            snprintf(errorStr, errorLen, "failed to read data (error: %d)", result);
            return false;
        }
        readPos += bytesRead;
        readLen -= bytesRead;
    }

    return true;
}
