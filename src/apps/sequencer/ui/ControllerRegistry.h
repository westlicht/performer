#pragma once

struct ControllerInfo {
    enum class Type : uint8_t {
        Launchpad,
    };

    uint16_t vendorId;
    uint16_t productId;
    Type type;
};

static const ControllerInfo controllerInfos[] = {
    { 0x1235, 0x0037, ControllerInfo::Type::Launchpad }   // Novation Launchpad Mini
};

static const ControllerInfo *findController(uint16_t vendorId, uint16_t productId) {
    for (size_t i = 0; i < sizeof(controllerInfos) / sizeof(controllerInfos[0]); ++i) {
        auto info = &controllerInfos[i];
        if (info->vendorId == vendorId && info->productId == productId) {
            return info;
        }
    }
    return nullptr;
}
