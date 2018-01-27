#include "os.h"

namespace os {

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

    DBG("---------------------------------------------");
    DBG("id name            bp cp stck free ttot trel");

    enumerate([&] (const TaskInfo &info) {
        TaskStatus_t taskStatus;
        vTaskGetInfo(info.handle, &taskStatus, pdTRUE, eRunning);

        DBG("%2d %-15s %2d %2d %4d %4d %3d%% %3d%%",
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
