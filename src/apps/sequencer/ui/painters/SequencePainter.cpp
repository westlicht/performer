#include "SequencePainter.h"

void SequencePainter::drawLoopStart(Canvas &canvas, int x, int y, int w) {
    canvas.vline(x, y - 1, 3);
    canvas.point(x + 1, y);
}

void SequencePainter::drawLoopEnd(Canvas &canvas, int x, int y, int w) {
    x += w - 1;
    canvas.vline(x, y - 1, 3);
    canvas.point(x - 1, y);
}

void SequencePainter::drawOffset(Canvas &canvas, int x, int y, int w, int h, int offset, int minOffset, int maxOffset) {
    auto remap = [w, minOffset, maxOffset] (int value) {
        return ((w - 1) * (value - minOffset)) / (maxOffset - minOffset);
    };

    canvas.setBlendMode(BlendMode::Set);

    canvas.setColor(0x7);
    canvas.fillRect(x, y, w, h);

    canvas.setColor(0);
    canvas.vline(x + remap(0), y, h);

    canvas.setColor(0xf);
    canvas.vline(x + remap(offset), y, h);
}

void SequencePainter::drawRetrigger(Canvas &canvas, int x, int y, int w, int h, int retrigger, int maxRetrigger) {
    canvas.setBlendMode(BlendMode::Set);

    int bw = w / maxRetrigger;
    x += (w - bw * retrigger) / 2;

    canvas.setColor(0xf);

    for (int i = 0; i < retrigger; ++i) {
        canvas.fillRect(x, y, bw / 2, h);
        x += bw;
    }
}

void SequencePainter::drawProbability(Canvas &canvas, int x, int y, int w, int h, int probability, int maxProbability) {
    canvas.setBlendMode(BlendMode::Set);

    int pw = (w * probability) / maxProbability;

    canvas.setColor(0xf);
    canvas.fillRect(x, y, pw, h);

    canvas.setColor(0x7);
    canvas.fillRect(x + pw, y, w - pw, h);
}

void SequencePainter::drawLength(Canvas &canvas, int x, int y, int w, int h, int length, int maxLength) {
    canvas.setBlendMode(BlendMode::Set);

    int gw = ((w - 1) * length) / maxLength;

    canvas.setColor(0xf);

    canvas.vline(x, y, h);
    canvas.hline(x, y, gw);
    canvas.vline(x + gw, y, h);
    canvas.hline(x + gw, y + h - 1, w - gw);
}

void SequencePainter::drawLengthRange(Canvas &canvas, int x, int y, int w, int h, int length, int range, int maxLength) {
    canvas.setBlendMode(BlendMode::Set);

    int gw = ((w - 1) * length) / maxLength;
    int rw = ((w - 1) * std::max(0, std::min(maxLength, length + range))) / maxLength;

    canvas.setColor(0x7);

    canvas.vline(x, y, h);
    canvas.hline(x, y, gw);
    canvas.vline(x + gw, y, h);
    canvas.hline(x + gw, y + h - 1, w - gw);

    canvas.setColor(0xf);

    canvas.fillRect(x + std::min(gw, rw), y + 2, std::max(gw, rw) - std::min(gw, rw) + 1, h - 4);
}

void SequencePainter::drawSlide(Canvas &canvas, int x, int y, int w, int h, bool active) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0xf);

    if (active) {
        canvas.line(x, y + h, x + w, y);
    } else {
        canvas.hline(x, y + h, w);
    }
}

void SequencePainter::drawSequenceProgress(Canvas &canvas, int x, int y, int w, int h, float progress) {
    if (progress < 0.f) {
        return;
    }

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(0x7);
    canvas.fillRect(x, y, w, h);
    canvas.setColor(0xf);
    canvas.vline(x + int(std::floor(progress * w)), y, h);
}
