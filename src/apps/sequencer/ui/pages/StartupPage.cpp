#include "StartupPage.h"

#include "ui/pages/Pages.h"

#include "model/FileManager.h"

#include "os/os.h"

StartupPage::StartupPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
    _startTicks = os::ticks();
}

void StartupPage::enter() {
    _engine.lock();

    FileManager::task([this] () {
        return FileManager::loadLastProject(_model.project());
    }, [this] (fs::Error result) {
        _engine.unlock();
        _ready = true;
    });
}

void StartupPage::draw(Canvas &canvas) {
    float time = (os::ticks() - _startTicks) / float(os::time::ms(1000));
    if (time > 1.f && _ready) {
        close();
    }

    canvas.setColor(0);
    canvas.fill();

    canvas.setColor(0xf);
    canvas.setFont(Font::Small);

    canvas.drawTextCentered(0, 16, Width, 32, "PERFORMER");
}

void StartupPage::updateLeds(Leds &leds) {
}
