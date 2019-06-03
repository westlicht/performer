#pragma once

#include "core/Debug.h"

#include "sim/Simulator.h"

#include <vector>
#include <functional>
#include <mutex>

namespace os {

    std::vector<std::function<void(void)>> &updateCallbacks();

    typedef int TaskHandle;

    template<size_t StackSize>
    class Task {
    public:
        Task(const char *name, uint8_t priority, std::function<void(void)> func) :
            _func(func)
        {
        }

        TaskHandle handle() const { return 0; }

    private:
        std::function<void(void)> _func;
    };

    template<size_t StackSize>
    class PeriodicTask {
    public:
        PeriodicTask(const char *name, uint8_t priority, uint32_t interval, std::function<void(void)> func) {
            os::updateCallbacks().emplace_back(func);
        }
    };

    inline void suspend(TaskHandle handle) {}
    inline void resume(TaskHandle handle) {}
    inline void resumeFromISR(TaskHandle handle) {}

    namespace this_task {

        inline TaskHandle handle() { return -1; }

        inline void suspend() {}
        inline void resume() {}
        inline void yield() {}

    } // namespace this_task


    class SemaphoreGeneric {
    public:
        bool take(uint32_t timeToWait = -1) {
            return false;
        }

        bool give() {
            return false;
        }

    protected:
        SemaphoreGeneric() {}
    };

    class Semaphore : public SemaphoreGeneric {
    public:
        Semaphore() {
            ASSERT(false, "not implemented");
        }
    };

    class CountingSemaphore : public SemaphoreGeneric {
    public:
        CountingSemaphore(uint32_t maxCount = -1, uint32_t initialCount = 0) {
            ASSERT(false, "not implemented");
        }
    };

    class Mutex {
    public:
        bool take(uint32_t timeToWait = -1) {
            ASSERT(timeToWait == uint32_t(-1), "cannot wait for mutex");
            _mutex.lock();
            return true;
        }

        bool give() {
            _mutex.unlock();
            return true;
        }

    private:
        std::mutex _mutex;
    };

    class RecursiveMutex : public SemaphoreGeneric {
    public:
        RecursiveMutex() {
            ASSERT(false, "not implemented");
        }
    };

    template<typename T, size_t Length>
    class Queue {
    public:
        Queue() {
            ASSERT(false, "not implemented");
        }

        void send(const T& element, uint32_t timeToWait = -1) {
        }

        void sendToBack(const T& element, uint32_t timeToWait = -1) {
        }

        void sendToFront(const T& element, uint32_t timeToWait = -1) {
        }

        bool peek(T * element, uint32_t timeToWait = -1) {
            return false;
        }

        bool receive(T * element, uint32_t timeToWait = -1) {
            return false;
        }

        T receive() {
            return T(0);
        }

    private:
        T _data[Length];
    };

    class InterruptLock {
    public:
        InterruptLock() {}
        ~InterruptLock() {}
    };

    namespace time {
        inline uint32_t us(uint32_t us) {
            return us / 1000;
        }
        inline uint32_t ms(uint32_t ms) {
            return ms;
        }
    };

    inline uint32_t ticks() {
        return sim::Simulator::instance().ticks();
    }

    inline void delay(uint32_t ticks) {
        uint32_t wakeupTick = os::ticks() + ticks;
        while (os::ticks() < wakeupTick) {}
    }

    inline void delayUntil(uint32_t &lastWakeupTime, uint32_t ticks) {
        uint32_t wakeupTick = os::ticks() + ticks;
        while (os::ticks() < wakeupTick) {}
    }

    inline void startScheduler() {
    }

} // namespace os
