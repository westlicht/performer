#include "Error.h"

namespace fs {

const char *errorToString(Error error) {
    switch (error) {
    case OK:                    return "OK";
    case DISK_ERR:              return "DISK_ERR";
    case INT_ERR:               return "INT_ERR";
    case NOT_READY:             return "NOT_READY";
    case NO_FILE:               return "NO_FILE";
    case NO_PATH:               return "NO_PATH";
    case INVALID_NAME:          return "INVALID_NAME";
    case DENIED:                return "DENIED";
    case EXIST:                 return "EXISTS";
    case INVALID_OBJECT:        return "INVALID_OBJECT";
    case WRITE_PROTECTED:       return "WRITE_PROTECTED";
    case INVALID_DRIVE:         return "INVALID_DRIVE";
    case NOT_ENABLED:           return "NOT_ENABLED";
    case NO_FILESYSTEM:         return "NO_FILESYSTEM";
    case MKFS_ABORTED:          return "MKFS_ABORTED";
    case TIMEOUT:               return "TIMEOUT";
    case LOCKED:                return "LOCKED";
    case NOT_ENOUGH_CORE:       return "NOT_ENOUGH_CORE";
    case TOO_MANY_OPEN_FILES:   return "TOO_MANY_OPEN_FILES";
    case INVALID_PARAMETER:     return "INVALID_PARAMETER";
    case DISK_FULL:             return "DISK_FULL";
    case END_OF_FILE:           return "END_OF_FILE";
    case INVALID_CHECKSUM:      return "INVALID_CHECKSUM";
    default:                    return "unknown";
    }
}

} // namespace fs
