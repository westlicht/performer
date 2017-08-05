#pragma once

namespace MatrixMap {

    // index = col * 8 + row

    //                              ROW
    //          0     1     2     3     4     5     6     7
    //        -----------------------------------------------
    //     5 | F-0 | F-1 | F-2 | F-3 | F-4 |                 |
    //  C  4 | xxx | xxx | xxx | xxx | xxx | xxx | xxx | xxx |
    //  O  2 | T-0 | T-1 | T-2 | T-3 | T-4 | T-5 | T-6 | T-7 |
    //  L  1 | S-0 | S-1 | S-2 | S-3 | S-4 | S-5 | S-6 | S-7 |
    //     0 | S-8 | S-9 | S10 | S11 | S12 | S13 | S14 | S15 |
    //        -----------------------------------------------

    static constexpr int fromStep(int step) {
        return step < 8 ? (8 + step) : (step - 8);
    }

    static constexpr bool isStep(int index) {
        return index >= fromStep(8) && index <= fromStep(7);
    }

    static constexpr int toStep(int index) {
        return index >= fromStep(0) ? (index - fromStep(0)) : (index - fromStep(8) + 8);
    }

    static constexpr int fromTrack(int track) {
        return 16 + track;
    }

    static constexpr bool isTrack(int index) {
        return index >= fromTrack(0) && index <= fromTrack(7);
    }

    static constexpr int toTrack(int index) {
        return index - fromTrack(0);
    }

    static constexpr int fromFunction(int function) {
        return 32 + function;
    }

    static constexpr int isFunction(int index) {
        return index >= fromFunction(0) && index <= fromFunction(4);
    }

    static constexpr int toFunction(int index) {
        return index - fromFunction(0);
    }

} // namespace MatrixMap
