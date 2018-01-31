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
}

void AsteroidsPage::exit() {
    _engine.setGateOutputOverride(false);
}

void AsteroidsPage::draw(Canvas &canvas) {
    uint32_t currentTicks = os::ticks();
    float dt = float(currentTicks - _lastTicks) / os::time::ms(1000);
    _lastTicks = currentTicks;

    asteroids::Inputs inputs = _keyInputs;
    asteroids::Outputs outputs;

    // const auto &adc = _engine.adc();
    // inputs.left     |= adc.channel(0) > 0x1fff;
    // inputs.right    |= adc.channel(1) > 0x1fff;
    // inputs.thrust   |= adc.channel(2) > 0x1fff;
    // inputs.shoot    |= adc.channel(3) > 0x1fff;

    _game.update(dt, inputs, outputs);
    _game.draw(canvas);

    _engine.setGateOutput(outputs.gates);
}

void AsteroidsPage::updateLeds(Leds &leds) {

}

void AsteroidsPage::keyDown(KeyEvent &event) {
    auto const &key = event.key();

    _keyInputs.left |= key.is(Key::F0);
    _keyInputs.right |= key.is(Key::F2);
    _keyInputs.thrust |= key.is(Key::F1);
    _keyInputs.shoot |= key.is(Key::F4);

    event.consume();
}

void AsteroidsPage::keyUp(KeyEvent &event) {
    auto const &key = event.key();

    _keyInputs.left &= !key.is(Key::F0);
    _keyInputs.right &= !key.is(Key::F2);
    _keyInputs.thrust &= !key.is(Key::F1);
    _keyInputs.shoot &= !key.is(Key::F4);

    event.consume();
}

void AsteroidsPage::encoder(EncoderEvent &event) {
    event.consume();
}
