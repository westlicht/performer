#include "Pattern.h"

void Pattern::write(WriteContext &context, int index) const {
    writeArray(context, _sequences);
}

void Pattern::read(ReadContext &context, int index) {
    readArray(context, _sequences);
}
