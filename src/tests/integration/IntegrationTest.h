#include "test/IntegrationTestRunner.h"

class IntegrationTest {
public:
    IntegrationTest(const char *name, bool interactive) :
        _name(name),
        _interactive(interactive)
    {}

    const char *name() const { return _name; }
    bool interactive() const { return _interactive; }

    virtual void init() {}
    virtual void once() {}
    virtual void update() {}

private:
    const char *_name;
    bool _interactive;
};

#define EXPECT(_cond_, _fmt_, ...)  \
    INTEGRATION_TEST_RUNNER_EXPECT(_cond_, _fmt_, ##__VA_ARGS__)

#define INTEGRATION_TEST(_class_)   \
    INTEGRATION_TEST_RUNNER(_class_)
