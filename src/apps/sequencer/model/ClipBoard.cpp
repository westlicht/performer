#include "ClipBoard.h"

ClipBoard::ClipBoard() {
    clear();
}

void ClipBoard::clear() {
    _trackBuffer.clear();
    _sequenceBuffer.clear();
    // _patternBuffer.clear();
}
