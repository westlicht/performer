#pragma once

#include "SystemConfig.h"

#include "core/Debug.h"

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
}

#include <functional>
#include <algorithm>

#include <libopencm3/cm3/cortex.h>

namespace os {

    typedef TaskHandle_t TaskHandle;

    class TaskProfiler {
    public:
        struct TaskInfo {
            struct TaskInfo *next = nullptr;
            TaskHandle handle;
            uint16_t stackSize;
            uint32_t lastRunTimeCounter;
            uint32_t runTime;
            uint32_t relativeRunTime;
        };

        static void registerTask(TaskInfo *taskInfo) {
            TaskInfo **tail = &_taskInfos;
            while (*tail != nullptr) {
                tail = &(*tail)->next;
            }
            *tail = taskInfo;
        }

        static void dump();

    private:
        template<typename Func>
        static void enumerate(Func func) {
            TaskInfo *info = _taskInfos;
            while (info) {
                func(*info);
                info = info->next;
            }
            _idleTaskInfo.handle = xTaskGetIdleTaskHandle();
            func(_idleTaskInfo);
        }

        static TaskInfo *_taskInfos;
        static TaskInfo _idleTaskInfo;
    };

    template<size_t StackSize>
    class Task {
    public:
        Task(const char *name, uint8_t priority, std::function<void(void)> func) :
            _func(func)
        {
            _handle = xTaskCreateStatic(&start, name, StackSize / sizeof(StackType_t), this, priority, _stack, &_task);

#if CONFIG_ENABLE_TASK_PROFILER
            _taskInfo.handle = _handle;
            _taskInfo.stackSize = StackSize;
            TaskProfiler::registerTask(&_taskInfo);
#endif // CONFIG_ENABLE_TASK_PROFILER
        }

        TaskHandle handle() const { return _handle; }

        const char *name() const { return pcTaskGetName(_handle); }

        size_t stackSize() const {
            return StackSize;
        }

    private:
        static void start(void *task) {
            reinterpret_cast<Task<StackSize> *>(task)->_func();
        }

        std::function<void(void)> _func;
        TaskHandle_t _handle;
        StaticTask_t _task;
        StackType_t _stack[StackSize / sizeof(StackType_t)];

#if CONFIG_ENABLE_TASK_PROFILER
        TaskProfiler::TaskInfo _taskInfo;
#endif // CONFIG_ENABLE_TASK_PROFILER

    };

    inline void suspend(TaskHandle handle) { vTaskSuspend(handle); }
    inline void resume(TaskHandle handle) { vTaskResume(handle); }
    inline void resumeFromISR(TaskHandle handle) { xTaskResumeFromISR(handle); }

    namespace this_task {

        inline TaskHandle handle() { return xTaskGetCurrentTaskHandle(); }

        inline void suspend() { os::suspend(handle()); }
        inline void resume() { os::suspend(handle()); }
        inline void yield() { taskYIELD(); }

    } // namespace this_task


    typedef BaseType_t TaskWoken;

    namespace isr {
        inline void exit(TaskWoken taskWoken) {
            portYIELD_FROM_ISR(taskWoken);
        }
    }


    class SemaphoreGeneric {
    public:
        bool take(uint32_t timeToWait = portMAX_DELAY) {
            return xSemaphoreTake(_handle, timeToWait) == pdTRUE;
        }

        TaskWoken takeFromISR() {
            TaskWoken taskWoken;
            xSemaphoreTakeFromISR(_handle, &taskWoken);
            return taskWoken;
        }

        bool give() {
            return xSemaphoreGive(_handle) == pdTRUE;
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

    class CountingSemaphore : public SemaphoreGeneric {
    public:
        CountingSemaphore(uint32_t maxCount = portMAX_DELAY, uint32_t initialCount = 0) {
            _handle = xSemaphoreCreateCountingStatic(maxCount, initialCount, &_semaphore);
        }
    };

    class Mutex : public SemaphoreGeneric {
    public:
        Mutex() {
            _handle = xSemaphoreCreateMutexStatic(&_semaphore);
        }
    };

    class RecursiveMutex : public SemaphoreGeneric {
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
        InterruptLock() {
            cm_disable_interrupts();
            ++_nestedCount;
        }

        ~InterruptLock() {
            if (--_nestedCount == 0) {
                cm_enable_interrupts();
            }
        }

    private:
        static uint32_t _nestedCount;
    };

    namespace time {
        constexpr inline uint32_t us(uint32_t us) {
            return (us * configTICK_RATE_HZ) / 1000000;
        }
        constexpr inline uint32_t ms(uint32_t ms) {
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


    template<size_t StackSize>
    class PeriodicTask : public Task<StackSize> {
    public:
        PeriodicTask(const char *name, uint8_t priority, uint32_t interval, std::function<void(void)> func) :
            Task<StackSize>(name, priority, [interval, func] () {
                uint32_t lastWakeupTime = os::ticks();
                while (true) {
                    func();
                    os::delayUntil(lastWakeupTime, interval);
                }
            })
        {
        }
    };

} // namespace os
