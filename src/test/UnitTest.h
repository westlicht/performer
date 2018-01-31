#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

#include "test/UnitTestRunner.h"
#include "Timer.h"

#include <cstddef>
#include <csetjmp>

#define UNIT_TEST_PRINTF(_fmt_, ...) UNIT_TEST_RUNNER_PRINTF(_fmt_, ##__VA_ARGS__)

void unitTest();

namespace test {

    struct Location {
        const char *file;
        int line;
    };

    struct RunState {
        enum Mode {
            Count,
            Run,
        };

        Mode mode;
        size_t index;
        size_t runIndex;
    };

    // static const char *name;
    static RunState runState;
    static jmp_buf jmpBuf;

    static void runCases() {
        unitTest();
    }

    static size_t countCases() {
        runState.mode = RunState::Count;
        runState.index = 0;
        runCases();
        return runState.index;
    }

    static bool runCase(size_t index) {
        runState.mode = RunState::Run;
        runState.index = 0;
        runState.runIndex = index;

        if (!setjmp(jmpBuf)) {
            runCases();
            return true;
        } else {
            return false;
        }
    }

    static bool isCaseActive(const char *name, const Location &location) {
        switch (runState.mode) {
        case RunState::Count:
            ++runState.index;
            return false;
        case RunState::Run:
            if (runState.index++ == runState.runIndex) {
                UNIT_TEST_PRINTF("Case: %s\n", name);
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    bool run(const char *name) {
        UNIT_TEST_PRINTF("\n========================================\n");
        UNIT_TEST_PRINTF("Unit Test: %s\n", name);
        size_t count = countCases();
        bool result = true;

        for (size_t i = 0; i < count; ++i) {
            UNIT_TEST_PRINTF("----------------------------------------\n");
            bool success = runCase(i);
            result &= success;
            if (success) {
                UNIT_TEST_PRINTF("Passed\n");
            }
        }

        UNIT_TEST_PRINTF("----------------------------------------\n");
        UNIT_TEST_PRINTF("Finished %s\n", result ? "successful" : "with failures");
        UNIT_TEST_PRINTF("========================================\n\n");

        return result;
    }

    template<typename Print>
    static void assert(bool condition, const Location &location, const char *msg, Print print) {
        if (!condition) {
            UNIT_TEST_PRINTF("Failed in %s:%d", location.file, location.line);
            if (msg) {
                UNIT_TEST_PRINTF(": %s\n", msg);
            } else {
                UNIT_TEST_PRINTF("\n");
            }
            print();
            longjmp(jmpBuf, 1);
        }
    }

    static void expect(bool condition, const Location &location, const char *msg = nullptr) {
        assert(condition, location, msg, [] () {});
    }

    static void expectTrue(bool condition, const Location &location, const char *msg = nullptr) {
        assert(condition, location, msg, [&] () { UNIT_TEST_PRINTF("is not true\n"); });
    }

    static void expectFalse(bool condition, const Location &location, const char *msg = nullptr) {
        assert(!condition, location, msg, [&] () { UNIT_TEST_PRINTF("is not false\n"); });
    }

    template<typename T>
    static void expectEqual(T a, T b, const Location &location, const char *msg = nullptr) {
        assert(a == b, location, msg, [] () {});
    }

    template<>
    void expectEqual<int>(int a, int b, const Location &location, const char *msg) {
        assert(a == b, location, msg, [&] () { UNIT_TEST_PRINTF("%d is not %d\n", a, b); });
    }

    template<>
    void expectEqual<float>(float a, float b, const Location &location, const char *msg) {
        assert(a == b, location, msg, [&] () { UNIT_TEST_PRINTF("%f is not %f\n", a, b); });
    }

    template<>
    void expectEqual<const char *>(const char *a, const char *b, const Location &location, const char *msg) {
        while (*a == *b && *a != '\0') {
            ++a;
            ++b;
        }
        assert(*b == '\0', location, msg, [&] () { UNIT_TEST_PRINTF("'%s' is not '%s'\n", a, b); });
    }

    static void expectEqual(char *a, const char *b, const Location &location, const char *msg = nullptr) {
        expectEqual(const_cast<const char *>(a), b, location, msg);
    }

    static void expectEqual(const char *a, char *b, const Location &location, const char *msg = nullptr) {
        expectEqual(a, const_cast<const char *>(b), location, msg);
    }

} // namespace test


#define UNIT_TEST_LOCATION { __FILE__, __LINE__ }

#define expect(_condition_, ...) test::expect(_condition_, UNIT_TEST_LOCATION, ##__VA_ARGS__)
#define expectTrue(_condition_, ...) test::expectTrue(_condition_, UNIT_TEST_LOCATION, ##__VA_ARGS__)
#define expectFalse(_condition_, ...) test::expectFalse(_condition_, UNIT_TEST_LOCATION, ##__VA_ARGS__)
#define expectEqual(_a_, _b_, ...) test::expectEqual(_a_, _b_, UNIT_TEST_LOCATION, ##__VA_ARGS__)

#define print(_fmt_, ...) UNIT_TEST_PRINTF(_fmt_, ##__VA_ARGS__)

#define UNIT_TEST(_name_)           \
    UNIT_TEST_RUNNER(_name_)        \
    void unitTest()

#define CASE(_name_)                \
    if (test::isCaseActive(_name_, UNIT_TEST_LOCATION))

#pragma GCC diagnostic pop
