#include "IntroPage.h"

#include "os/os.h"

IntroPage::IntroPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
    _lastTicks = os::ticks();
}

void IntroPage::draw(Canvas &canvas) {
    uint32_t currentTicks = os::ticks();
    float dt = float(currentTicks - _lastTicks) / os::time::ms(1000);
    _lastTicks = currentTicks;

    _intro.update(dt);
    _intro.draw(canvas);
}
