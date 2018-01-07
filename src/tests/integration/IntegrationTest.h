#include "test/IntegrationTestRunner.h"

class IntegrationTest {
public:
    virtual void init() {}
    virtual void update() { _terminate = true; }

    bool terminate() const { return _terminate; }

private:
    bool _terminate = false;
};

#define EXPECT(_cond_, _fmt_, ...) \
    INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ##__VA_ARGS__)

#define INTEGRATION_TEST(_name_, _class_)   \
    INTEGRATION_TEST_RUNNER(_name_, _class_)
