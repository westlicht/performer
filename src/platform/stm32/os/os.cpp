#include "os.h"

namespace os {

uint32_t InterruptLock::_nestedCount = 0;

#if CONFIG_ENABLE_TASK_PROFILER
TaskProfiler::TaskInfo *TaskProfiler::_taskInfos;
TaskProfiler::TaskInfo TaskProfiler::_idleTaskInfo;

void TaskProfiler::dump() {
    uint32_t totalRunTime = 0;
    uint32_t totalRelativeRunTime = 0;

    enumerate([&] (TaskInfo &info) {
        TaskStatus_t taskStatus;
        vTaskGetInfo(info.handle, &taskStatus, pdFALSE, eRunning);

        uint32_t runTimeCounter = taskStatus.ulRunTimeCounter;
        uint32_t relativeRunTimeCounter = runTimeCounter - info.lastRunTimeCounter;
        info.lastRunTimeCounter = runTimeCounter;

        info.runTime = runTimeCounter;
        info.relativeRunTime = relativeRunTimeCounter;

        totalRunTime += runTimeCounter;
        totalRelativeRunTime += relativeRunTimeCounter;
    });

    totalRunTime = std::max(1ul, totalRunTime / 100);
    totalRelativeRunTime = std::max(1ul, totalRelativeRunTime / 100);

    DBG("Task Profiler:");
    DBG("---------------------------------------------");
    DBG("id name            bp cp stck free ttot trel");

    enumerate([&] (const TaskInfo &info) {
        TaskStatus_t taskStatus;
        vTaskGetInfo(info.handle, &taskStatus, pdTRUE, eRunning);

        DBG("%2ld %-15s %2ld %2ld %4zd %4hd %3ld%% %3ld%%",
            taskStatus.xTaskNumber,
            taskStatus.pcTaskName,
            taskStatus.uxBasePriority,
            taskStatus.uxCurrentPriority,
            info.stackSize,
            taskStatus.usStackHighWaterMark,
            info.runTime / totalRunTime,
            info.relativeRunTime / totalRelativeRunTime
        );
    });

    DBG("---------------------------------------------");
}

#endif // CONFIG_ENABLE_TASK_PROFILER

} // namespace os
