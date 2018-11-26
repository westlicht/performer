#pragma once

#include "test/IntegrationTestRunner.h"
#include "Timer.h"

class IntegrationTest {
public:
    virtual ~IntegrationTest() {}
    virtual void init() {}
    virtual void once() {}
    virtual void update() {}
};

#define EXPECT(_cond_, _fmt_, ...) \
    INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ##__VA_ARGS__)

#define INTEGRATION_TEST(_class_, _name_, _interactive_) \
    INTEGRATION_TEST_RUNNER(_class_, _name_, _interactive_)
