#include "os.h"

namespace os {

std::vector<std::function<void(void)>> &updateCallbacks() {
    static std::vector<std::function<void(void)>> callbacks;
    return callbacks;
}

} // namespace os
