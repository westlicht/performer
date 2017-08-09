#pragma once

#include "core/Debug.h"

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
}

#include <functional>

#include <libopencm3/cm3/cortex.h>

namespace os {

    typedef TaskHandle_t TaskHandle;

    template<size_t StackSize>
    class Task {
    public:
        Task(const char *name, uint8_t priority, std::function<void(void)> func) :
            _func(func)
        {
            // TODO not really necessary for release
            for (size_t i = 0; i < StackSize / sizeof(StackType_t); ++i) {
                _stack[i] = StackType_t(0xdeadbeef);
            }
            _handle = xTaskCreateStatic(&start, name, StackSize / sizeof(StackType_t), this, priority, _stack, &_task);
        }

        TaskHandle handle() const { return _handle; }

        const char *name() const { return pcTaskGetName(_handle); }

        size_t stackSize() const {
            return StackSize;
        }

        size_t stackUsage() const {
            for (size_t i = 0; i < StackSize / sizeof(StackType_t); ++i) {
                if (_stack[i] != StackType_t(0xdeadbeef)) {
                    return StackSize - i * sizeof(StackType_t);
                }
            }
            return 0;
        }

    private:
        static void start(void *task) {
            reinterpret_cast<Task<StackSize> *>(task)->_func();
        }

        std::function<void(void)> _func;
        TaskHandle_t _handle;
        StaticTask_t _task;
        StackType_t _stack[StackSize / sizeof(StackType_t)];
    };

    inline void suspend(TaskHandle handle) { vTaskSuspend(handle); }
    inline void resume(TaskHandle handle) { vTaskResume(handle); }
    inline void resumeFromISR(TaskHandle handle) { xTaskResumeFromISR(handle); }

    namespace this_task {

        inline TaskHandle handle() { return xTaskGetCurrentTaskHandle(); }

        inline void suspend() { os::suspend(handle()); }
        inline void resume() { os::suspend(handle()); }

    } // namespace this_task


    typedef BaseType_t TaskWoken;

    namespace isr {
        inline void exit(TaskWoken taskWoken) {
            portYIELD_FROM_ISR(taskWoken);
        }
    }


    class SemaphoreGeneric {
    public:
        void take(uint32_t timeToWait = portMAX_DELAY) {
            xSemaphoreTake(_handle, timeToWait);
        }

        TaskWoken takeFromISR() {
            TaskWoken taskWoken;
            xSemaphoreTakeFromISR(_handle, &taskWoken);
            return taskWoken;
        }

        void give() {
            xSemaphoreGive(_handle);
        }

        TaskWoken giveFromISR() {
            TaskWoken taskWoken;
            xSemaphoreGiveFromISR(_handle, &taskWoken);
            return taskWoken;
        }

        // ~SemaphoreGeneric() {
        //     vSemaphoreDelete(_handle);
        // }
    protected:
        SemaphoreGeneric() {}

        SemaphoreHandle_t _handle;
        StaticSemaphore_t _semaphore;
    };

    class Semaphore : public SemaphoreGeneric {
    public:
        Semaphore() {
            _handle = xSemaphoreCreateBinaryStatic(&_semaphore);
        }
    };

    class CountingSemaphore : SemaphoreGeneric {
    public:
        CountingSemaphore(uint32_t maxCount = portMAX_DELAY, uint32_t initialCount = 0) {
            _handle = xSemaphoreCreateCountingStatic(maxCount, initialCount, &_semaphore);
        }
    };

    class Mutex : SemaphoreGeneric {
    public:
        Mutex() {
            _handle = xSemaphoreCreateMutexStatic(&_semaphore);
        }
    };

    class RecursiveMutex : SemaphoreGeneric {
    public:
        RecursiveMutex() {
            _handle = xSemaphoreCreateRecursiveMutexStatic(&_semaphore);
        }
    };

    template<typename T, size_t Length>
    class Queue {
    public:
        Queue() {
            _handle = xQueueCreateStatic(Length, sizeof(T), reinterpret_cast<uint8_t *>(_data), &_queue);
        }

        void send(const T& element, uint32_t timeToWait = portMAX_DELAY) {
            xQueueSend(_handle, &element, timeToWait);
        }

        void sendToBack(const T& element, uint32_t timeToWait = portMAX_DELAY) {
            xQueueSendToBack(_handle, &element, timeToWait);
        }

        void sendToFront(const T& element, uint32_t timeToWait = portMAX_DELAY) {
            xQueueSendToFront(_handle, &element, timeToWait);
        }

        bool peek(T * element, uint32_t timeToWait = portMAX_DELAY) {
            return xQueuePeek(_handle, element, timeToWait);
        }

        bool receive(T * element, uint32_t timeToWait = portMAX_DELAY) {
            return xQueueReceive(_handle, element, timeToWait);
        }

        T receive() {
            T element;
            xQueueReceive(_handle, &element, portMAX_DELAY);
            return element;
        }

    private:
        xQueueHandle _handle;
        StaticQueue_t _queue;
        T _data[Length];
    };

    class InterruptLock {
    public:
        InterruptLock() { cm_disable_interrupts(); }
        ~InterruptLock() { cm_enable_interrupts(); }
    };

    namespace time {
        inline uint32_t us(uint32_t us) {
            return (us * configTICK_RATE_HZ) / 1000000;
        }
        inline uint32_t ms(uint32_t ms) {
            return (ms * configTICK_RATE_HZ) / 1000;
        }
    };

    inline uint32_t ticks() {
        return xTaskGetTickCount();
    }

    inline void delay(uint32_t ticks) {
        vTaskDelay(ticks);
    }

    inline void delayUntil(uint32_t &lastWakeupTime, uint32_t ticks) {
        vTaskDelayUntil(&lastWakeupTime, ticks);
    }

    inline void startScheduler() {
        vTaskStartScheduler();
    }

} // namespace os
