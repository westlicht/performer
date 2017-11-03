#pragma once

#include "PageContext.h"

#include "ui/Page.h"
#include "ui/painters/WindowPainter.h"

#include "model/Model.h"

#include "engine/Engine.h"

#include "core/utils/StringBuilder.h"

class ValuePage : public Page {
public:
    ValuePage(PageManager &pageManager, PageContext &context) :
        Page(pageManager),
        _model(context.model),
        _engine(context.engine)
    {}

    struct Handler {
        Key::Code exitKey;
        std::function<void(StringBuilder<32> &)> value;
        std::function<void(int, bool)> encoder;
    };

    void show(const Handler &handler) {
        _handler = handler;
        _pageManager.push(this);
    }

    virtual void enter() override {
    }

    virtual void exit() override {
    }

    virtual void draw(Canvas &canvas) override {
        WindowPainter::drawFrame(canvas, 10, 16, 256 - 20, 32);

        canvas.setBlendMode(BlendMode::Set);
        canvas.setFont(Font::Small);
        StringBuilder<32> string;
        _handler.value(string);
        canvas.drawText(50, 36, string);
    }

    virtual void updateLeds(Leds &leds) override {
    }

    virtual void keyDown(KeyEvent &event) override {
        if (event.key().isGlobal()) {
            return;
        }

        event.consume();
    }

    virtual void keyUp(KeyEvent &event) override {
        if (event.key().isGlobal()) {
            return;
        }

        if (event.key().is(_handler.exitKey)) {
            close();
        }

        event.consume();
    }

    virtual void encoder(EncoderEvent &event) override {
        _handler.encoder(event.value(), event.pressed());
        event.consume();
    }

private:
    Model &_model;
    Engine &_engine;
    Handler _handler;
};
