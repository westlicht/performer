#include "ClipBoard.h"

ClipBoard::ClipBoard() {
    clear();
}

void ClipBoard::clear() {
    _trackSetupBuffer.clear();
    _sequenceBuffer.clear();
    // _patternBuffer.clear();
}
