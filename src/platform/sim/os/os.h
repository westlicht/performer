#pragma once

#include <functional>

namespace os {

    typedef int TaskHandle;

    template<size_t StackSize>
    class Task {
    public:
        Task(std::function<void(void)> func, uint8_t priority) :
            _func(func)
        {
        }

        TaskHandle handle() const { return 0; }

    private:
        std::function<void(void)> _func;
    };

    inline void suspend(TaskHandle handle) {}
    inline void resume(TaskHandle handle) {}
    inline void resumeFromISR(TaskHandle handle) {}

    namespace this_task {

        inline TaskHandle handle() { return -1; }

        inline void suspend() {}
        inline void resume() {}

    } // namespace this_task


    class SemaphoreGeneric {
    public:
        void take(uint32_t timeToWait = -1) {
        }

        void give() {
        }

    protected:
        SemaphoreGeneric() {}
    };

    class Semaphore : public SemaphoreGeneric {
    public:
        Semaphore() {
        }
    };

    class CountingSemaphore : SemaphoreGeneric {
    public:
        CountingSemaphore(uint32_t maxCount = -1, uint32_t initialCount = 0) {
        }
    };

    class Mutex : SemaphoreGeneric {
    public:
        Mutex() {
        }
    };

    class RecursiveMutex : SemaphoreGeneric {
    public:
        RecursiveMutex() {
        }
    };

    template<typename T, size_t Length>
    class Queue {
    public:
        Queue() {
        }

        void send(const T& element, uint32_t timeToWait = -1) {
        }

        void sendToBack(const T& element, uint32_t timeToWait = -1) {
        }

        void sendToFront(const T& element, uint32_t timeToWait = -1) {
        }

        bool peek(T * element, uint32_t timeToWait = -1) {
        }

        bool receive(T * element, uint32_t timeToWait = -1) {
        }

        T receive() {
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
            // return (us * configTICK_RATE_HZ) / 1000000;
        }
        inline uint32_t ms(uint32_t ms) {
            return ms;
            // return (ms * configTICK_RATE_HZ) / 1000;
        }
    };

    inline uint32_t ticks() {
        return 0;
    }

    inline void delay(uint32_t ticks) {
    }

    inline void delayUntil(uint32_t &lastWakeupTime, uint32_t ticks) {
    }

    inline void startScheduler() {
    }

} // namespace os
