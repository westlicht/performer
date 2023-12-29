#include "WindowPainter.h"

#include "Config.h"

#include "core/utils/StringBuilder.h"

static void drawInvertedText(Canvas &canvas, int x, int y, const char *text, bool inverted = true) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);

    if (inverted) {
        canvas.fillRect(x - 1, y - 5, canvas.textWidth(text) + 1, 7);
        canvas.setBlendMode(BlendMode::Sub);
    }

    canvas.drawText(x, y, text);
}

void WindowPainter::clear(Canvas &canvas) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::None);
    canvas.fill();
}

void WindowPainter::drawFrame(Canvas &canvas, int x, int y, int w, int h) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::None);
    canvas.fillRect(x, y, w, h);
    canvas.setColor(Color::Bright);
    canvas.drawRect(x, y, w, h);
}

void WindowPainter::drawFunctionKeys(Canvas &canvas, const char *names[], const KeyState &keyState, int highlight) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Medium);
    canvas.hline(0, PageHeight - FooterHeight - 1, PageWidth);

    for (int i = 0; i < FunctionKeyCount; ++i) {
        if (names[i] || (i + 1 < FunctionKeyCount && names[i + 1])) {
            int x = (PageWidth * (i + 1)) / FunctionKeyCount;
            canvas.vline(x, PageHeight - FooterHeight, FooterHeight);
        }
    }

    canvas.setFont(Font::Tiny);

    for (int i = 0; i < FunctionKeyCount; ++i) {
        canvas.setColor(Color::Medium);
        if (names[i]) {
            bool pressed = keyState[Key::F0 + i];

            if (highlight >= 0) {
                pressed = i == highlight;
            }

            int x0 = (PageWidth * i) / FunctionKeyCount;
            int x1 = (PageWidth * (i + 1)) / FunctionKeyCount;
            int w = x1 - x0 + 1;

            if (pressed) {
                canvas.setColor(Color::Bright);
            }

            canvas.drawText(x0 + (w - canvas.textWidth(names[i])) / 2, PageHeight - 3, names[i]);
        }
    }
}

void WindowPainter::drawClock(Canvas &canvas, const Engine &engine) {
    static const char *clockModeName[] = { "A", "M", "S" };
    const char *name = engine.recording() ? "R" : clockModeName[int(engine.clock().activeMode())];

    drawInvertedText(canvas, 2, 8 - 2, name);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);
    canvas.drawText(10, 8 - 2, FixedStringBuilder<8>("%.1f", engine.tempo()));
}

void WindowPainter::drawActiveState(Canvas &canvas, int track, int playPattern, int editPattern, bool snapshotActive, bool songActive) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);

    // draw selected track
    canvas.drawText(40, 8 - 2, FixedStringBuilder<8>("T%d", track + 1));

    if (snapshotActive) {
        drawInvertedText(canvas, 56, 8 - 2, "SNAP", true);
    } else {
        // draw active pattern
        drawInvertedText(canvas, 56, 8 - 2, FixedStringBuilder<8>("P%d", playPattern + 1), songActive);

        // draw edit pattern
        drawInvertedText(canvas, 75, 8 - 2, FixedStringBuilder<8>("E%d", editPattern + 1), playPattern == editPattern);
    }
}

void WindowPainter::drawActiveMode(Canvas &canvas, const char *mode) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);
    canvas.drawText(PageWidth - canvas.textWidth(mode) - 2, 8 - 2, mode);
}

void WindowPainter::drawActiveFunction(Canvas &canvas, const char *function) {
    canvas.setFont(Font::Tiny);
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Bright);
    canvas.drawText(100, 8 - 2, function);
}

void WindowPainter::drawHeader(Canvas &canvas, const Model &model, const Engine &engine, const char *mode) {
    const auto &project = model.project();
    int track = project.selectedTrackIndex();
    int playPattern = project.playState().trackState(track).pattern();
    int editPattern = project.selectedPatternIndex();
    bool snapshotActive = project.playState().snapshotActive();
    bool songActive = project.playState().songState().playing();

    drawClock(canvas, engine);
    drawActiveState(canvas, track, playPattern, editPattern, snapshotActive, songActive);
    drawActiveMode(canvas, mode);

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Medium);
    canvas.hline(0, HeaderHeight, PageWidth);
}

void WindowPainter::drawFooter(Canvas &canvas) {
    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Medium);
    canvas.hline(0, PageHeight - FooterHeight - 1, PageWidth);
}

void WindowPainter::drawFooter(Canvas &canvas, const char *names[], const KeyState &keyState, int highlight) {
    drawFunctionKeys(canvas, names, keyState, highlight);
}

void WindowPainter::drawScrollbar(Canvas &canvas, int x, int y, int w, int h, int totalRows, int visibleRows, int displayRow) {
    if (visibleRows >= totalRows) {
        return;
    }

    canvas.setBlendMode(BlendMode::Set);
    canvas.setColor(Color::Medium);
    canvas.drawRect(x, y, w, h);

    int bh = (visibleRows * h) / totalRows;
    int by = (displayRow * h) / totalRows;
    canvas.setColor(Color::Bright);
    canvas.fillRect(x, y + by, w, bh);
}
