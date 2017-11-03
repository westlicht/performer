#pragma once

#include "SystemConfig.h"

#include "drivers/HighResolutionTimer.h"

#include <cstdint>

#if CONFIG_ENABLE_PROFILER

class Profiler {
public:
    static void init();
    static void dump();

    struct Interval {
        Interval(const char *desc) : desc(desc) {
            registerInterval(this);
        }

        inline void begin() {
            start = HighResolutionTimer::us();
        }

        inline void end() {
            uint32_t end = HighResolutionTimer::us();
            avg = end - start;
        }

        const char *desc;
        uint32_t start;
        uint32_t avg;
    };

    struct Counter {
        Counter(const char *desc) : desc(desc) {
            registerCounter(this);
        }

        inline void add(int num = 1) {
            count += num;
        }

        const char *desc;
        uint32_t count;
    };

private:
    static const int MaxIntervals = 16;
    static const int MaxCounters = 16;

    static void registerInterval(Interval *interval);
    static void registerCounter(Counter *counter);

    static int _numIntervals;
    static int _numCounters;
    static Interval *_intervals[MaxIntervals];
    static Counter *_counters[MaxCounters];
};

# define PROFILER_INTERVAL(_name_, _desc_) \
    static Profiler::Interval _name_##_profiler_interval(_desc_);
# define PROFILER_INTERVAL_BEGIN(_name_) \
    _name_##_profiler_interval.begin();
# define PROFILER_INTERVAL_END(_name_) \
    _name_##_profiler_interval.end();

# define PROFILER_COUNTER(_name_, _desc_) \
    static Profiler::Counter _name_##_profiler_counter(_desc_);
# define PROFILER_COUNTER_ADD(_num_) \
    _name_##_profiler_counter.add(_num_);

#else // CONFIG_ENABLE_PROFILER

class Profiler {
public:
    static void init() {}
    static void dump() {}
};

# define PROFILER_INTERVAL(_name_, _desc_)
# define PROFILER_INTERVAL_BEGIN(_name_)
# define PROFILER_INTERVAL_END(_name_)

# define PROFILER_COUNTER(_name_, _desc_)
# define PROFILER_COUNTER_ADD(_num_)

#endif // CONFIG_ENABLE_PROFILER
