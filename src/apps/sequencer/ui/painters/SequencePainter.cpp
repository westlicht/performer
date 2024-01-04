#include "SequencePainter.h"
#include "core/gfx/Canvas.h"
#include "model/NoteSequence.h"
#include <bitset>

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

    canvas.setColor(Color::Medium);
    canvas.fillRect(x, y, w, h);

    canvas.setColor(Color::None);
    canvas.vline(x + remap(0), y, h);

    canvas.setColor(Color::Bright);
    canvas.vline(x + remap(offset), y, h);
}

void SequencePainter::drawRetrigger(Canvas &canvas, int x, int y, int w, int h, int retrigger, int maxRetrigger) {
    canvas.setBlendMode(BlendMode::Set);

    int bw = w / maxRetrigger;
    x += (w - bw * retrigger) / 2;

    canvas.setColor(Color::Bright);

    for (int i = 0; i < retrigger; ++i) {
        canvas.fillRect(x, y, bw / 2, h);
        x += bw;
    }
}

void SequencePainter::drawProbability(Canvas &canvas, int x, int y, int w, int h, int probability, int maxProbability) {
    canvas.setBlendMode(BlendMode::Set);

    int pw = (w * probability) / maxProbability;

    canvas.setColor(Color::Bright);
    canvas.fillRect(x, y, pw, h);

    canvas.setColor(Color::Medium);
    canvas.fillRect(x + pw, y, w - pw, h);
}

void SequencePainter::drawLength(Canvas &canvas, int x, int y, int w, int h, int length, int maxLength) {
    canvas.setBlendMode(BlendMode::Set);

    int gw = ((w - 1) * length) / maxLength;

    canvas.setColor(Color::Bright);

    canvas.vline(x, y, h);
    canvas.hline(x, y, gw);
    canvas.vline(x + gw, y, h);
    canvas.hline(x + gw, y + h - 1, w - gw);
}

void SequencePainter::drawLengthRange(Canvas &canvas, int x, int y, int w, int h, int length, int range, int maxLength) {
    canvas.setBlendMode(BlendMode::Set);

    int gw = ((w - 1) * length) / maxLength;
    int rw = ((w - 1) * std::max(0, std::min(maxLength, length + range))) / maxLength;

    canvas.setColor(Color::Medium);

    canvas.vline(x, y, h);
    canvas.hline(x, y, gw);
    canvas.vline(x + gw, y, h);
    canvas.hline(x + gw, y + h - 1, w - gw);

    canvas.setColor(Color::Bright);

    canvas.fillRect(x + std::min(gw, rw), y + 2, std::max(gw, rw) - std::min(gw, rw) + 1, h - 4);
}

void SequencePainter::drawSlide(Canvas &canvas, int x, int y, int w, int h, bool active) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);

    if (active) {
        canvas.line(x, y + h, x + w, y);
    } else {
        canvas.hline(x, y + h, w);
    }
}

const std::bitset<4> mask = 0x1;
void SequencePainter::drawStageRepeatMode(Canvas &canvas, int x, int y, int w, int h, NoteSequence::StageRepeatMode mode) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Bright);
    int bottom = y + h - 1;
    std::bitset<4> enabled;
    x += (w - 8) / 2;

    switch (mode) {
        case NoteSequence::StageRepeatMode::Each:
           enabled = 0xf;
            break;
        case NoteSequence::StageRepeatMode::First:
            enabled = 0x1;
            break;
        case NoteSequence::StageRepeatMode::Middle:
            enabled = 0x1 << 2;
            break;
        case NoteSequence::StageRepeatMode::Last:
            enabled = 0x8;
            break;
        case NoteSequence::StageRepeatMode::Odd:
            enabled = 0x5;
            break;
        case NoteSequence::StageRepeatMode::Even:
            enabled = 0x5 << 1;
            break;
        case NoteSequence::StageRepeatMode::Triplets:
            enabled = 0x9;
            break;
        case NoteSequence::StageRepeatMode::Random:
            enabled = 0xf;
            break;
    }

    for (int i = 0; i < 4; i++) {
        if (mode == NoteSequence::StageRepeatMode::Random) {
            canvas.drawTextCentered(x,y, x+6, h, "????");
        } else {
            if (((enabled >> i) & mask) == 1) {
                canvas.vline(x + 2 * i, y, h);
            } else {
                canvas.hline(x + 2 * i, bottom, 1);
            }
        }
    }
}

void SequencePainter::drawSequenceProgress(Canvas &canvas, int x, int y, int w, int h, float progress) {
    if (progress < 0.f) {
        return;
    }

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Medium);
    canvas.fillRect(x, y, w, h);
    canvas.setColor(Color::Bright);
    canvas.vline(x + int(std::floor(progress * w)), y, h);
}
