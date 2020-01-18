#include "AsteroidsPage.h"

#include "os/os.h"

AsteroidsPage::AsteroidsPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{}

void AsteroidsPage::enter() {
    _lastTicks = os::ticks();
    _game.init();

    _engine.setGateOutputOverride(true);
    _engine.setGateOutput(0);
    _engine.suspend();
}

void AsteroidsPage::exit() {
    _engine.setGateOutputOverride(false);
    _engine.resume();
}

void AsteroidsPage::draw(Canvas &canvas) {
    uint32_t currentTicks = os::ticks();
    float dt = float(currentTicks - _lastTicks) / os::time::ms(1000);
    _lastTicks = currentTicks;

    const auto &cvInput = _engine.cvInput();
    asteroids::Inputs cvInputs;
    cvInputs.left     |= cvInput.channel(0) > 1.f;
    cvInputs.right    |= cvInput.channel(1) > 1.f;
    cvInputs.thrust   |= cvInput.channel(2) > 1.f;
    cvInputs.shoot    |= cvInput.channel(3) > 1.f;

    asteroids::Inputs inputs;
    inputs.keys = _keyInputs.keys | cvInputs.keys;
    asteroids::Outputs outputs;

    _game.update(dt, inputs, outputs);
    _game.draw(canvas);

    _engine.setGateOutput(outputs.gates);
}

void AsteroidsPage::updateLeds(Leds &leds) {

}

void AsteroidsPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    _keyInputs.left |= key.is(Key::F0);
    _keyInputs.right |= key.is(Key::F2);
    _keyInputs.thrust |= key.is(Key::F1);
    _keyInputs.shoot |= key.is(Key::F4);

    event.consume();
}

void AsteroidsPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    _keyInputs.left &= !key.is(Key::F0);
    _keyInputs.right &= !key.is(Key::F2);
    _keyInputs.thrust &= !key.is(Key::F1);
    _keyInputs.shoot &= !key.is(Key::F4);

    event.consume();
}

void AsteroidsPage::keyPress(KeyPressEvent &event) {
    const auto &key = event.key();

    if (key.pageModifier() && key.is(Key::Step15)) {
        close();
    }
}

void AsteroidsPage::encoder(EncoderEvent &event) {
    event.consume();
}
