#pragma once

namespace os {

class LockGuard {
public:
    LockGuard(os::Mutex &mutex) :
        _mutex(mutex)
    {
        _mutex.take();
    }

    ~LockGuard() {
        _mutex.give();
    }

private:
    Mutex &_mutex;
};

} // namespace os
