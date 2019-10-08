#include "TempoPage.h"

#include "ui/painters/WindowPainter.h"

#include "core/utils/StringBuilder.h"

TempoPage::TempoPage(PageManager &manager, PageContext &context) :
    BasePage(manager, context)
{
}

void TempoPage::enter() {
    _engine.tapTempoReset();
}

void TempoPage::exit() {
    _engine.nudgeTempoSetDirection(0);
}

void TempoPage::draw(Canvas &canvas) {
    WindowPainter::drawFrame(canvas, 16, 16, 256 - 32, 32);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setFont(Font::Small);
    canvas.setColor(0xf);

    if (globalKeyState()[Key::Pattern]) {
      FixedStringBuilder<16> swingString("Swing: %i%%", _project.swing());
      canvas.drawText(50, 34, swingString);
    }
    else {
      FixedStringBuilder<16> tempoString("Tempo: %.1f", _project.tempo());
      canvas.drawText(50, 34, tempoString);
    }

    float nudgeTempoStrength = _engine.nudgeTempoStrength();

    FixedStringBuilder<16> nudgeString("%.1f%%", nudgeTempoStrength * 10.f);

    canvas.setFont(Font::Tiny);

    if (nudgeTempoStrength > 0.f) {
        int w = nudgeTempoStrength * 10;
        canvas.fillRect(180, 32 - 2, w, 4);
        canvas.drawText(200, 32 + 1, nudgeString);
    } else if (nudgeTempoStrength < 0.f) {
        int w = std::abs(nudgeTempoStrength * 10);
        canvas.fillRect(180 - w, 32 - 2, w, 4);
        canvas.drawText(200, 32 + 1, nudgeString);
    }
}

void TempoPage::updateLeds(Leds &leds) {
}

void TempoPage::keyDown(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isPlay()) {
        // tap tempo
        _engine.tapTempoTap();
    } else if (key.isLeft()) {
        // nudge tempo down
        _engine.nudgeTempoSetDirection(-1);
    } else if (key.isRight()) {
        // nudge tempo up
        _engine.nudgeTempoSetDirection(1);
    }

    event.consume();
}

void TempoPage::keyUp(KeyEvent &event) {
    const auto &key = event.key();

    if (key.isTempo()) {
        close();
    } else if (key.isLeft() || key.isRight()) {
        // reset nudge
        _engine.nudgeTempoSetDirection(0);
    }

    event.consume();
}

void TempoPage::encoder(EncoderEvent &event) {

  if (globalKeyState()[Key::Pattern]) {
    _project.setSwing(_project.swing() + event.value() * (event.pressed() ? 0.1f : 1.f));
  } else {
    _project.setTempo(_project.tempo() + event.value() * (event.pressed() ? 0.1f : 1.f) * (globalKeyState()[Key::Shift] ? 10.f : 1.f));
  }
    event.consume();
}
