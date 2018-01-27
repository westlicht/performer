#include "Profiler.h"

#include "core/Debug.h"

#if CONFIG_ENABLE_PROFILER
int Profiler::_numIntervals;
int Profiler::_numCounters;
Profiler::Interval *Profiler::_intervals[Profiler::MaxIntervals];
Profiler::Counter *Profiler::_counters[Profiler::MaxCounters];

void Profiler::init() {
}

void Profiler::dump() {
    DBG("Profiler:");
    DBG("---------------------------------------------");
    if (_numIntervals > 0) {
        DBG("Intervals:");
        for (int i = 0; i < _numIntervals; ++i) {
            const auto &interval = *_intervals[i];
            DBG("  %s: %lu us", interval.desc, interval.avg);
        }
    }
    if (_numCounters > 0) {
        DBG("Counters:");
        for (int i = 0; i < _numCounters; ++i) {
            const auto &counter = *_counters[i];
            DBG("  %s: %lu", counter.desc, counter.count);
        }
    }
    DBG("---------------------------------------------");
}

void Profiler::registerInterval(Interval *interval) {
    if (_numIntervals < MaxIntervals) {
        _intervals[_numIntervals++] = interval;
    } else {
        DBG("Profiler: Too many profiler intervals!");
    }
}

void Profiler::registerCounter(Counter *counter) {
    if (_numCounters < MaxCounters) {
        _counters[_numCounters++] = counter;
    } else {
        DBG("Profiler: Too many profiler counters");
    }
}

#endif // CONFIG_ENABLE_PROFILER
