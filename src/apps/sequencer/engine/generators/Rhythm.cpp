#include "Rhythm.h"

namespace Rhythm {

// based on https://bitbucket.org/sjcastroe/bjorklunds-algorithm
Pattern euclidean(int beats, int steps) {
    // make sure beats <= steps
    beats = std::min(beats, steps);

    Pattern x;
    x.set(0, true);
    int xCount = beats;

    Pattern y;
    y.set(0, false);
    int yCount = steps - beats;

    // each iteration is a process of pairing strings x and y and the remainder from the pairings
    // x will hold the "dominant" pair (the pair that there are more of)
    do {
        Pattern yCopy = y;

        // check which is the dominant pair
        if (xCount >= yCount) {
            // set the new number of pairs for x and y
            int yCountNew = xCount - yCount;
            xCount = yCount;
            yCount = yCountNew;

            // the previous dominant pair becomes the new non-dominant pair
            y = x;
        } else {
            yCount -= xCount;
        }

        // create the new dominant pair by combining the previous pairs
        x.append(yCopy);

        // iterate as long as the non-dominant pair can be paired (if there is 1 y left, all we can do is pair it with however many x are left, so we're done)
    } while (xCount > 1 && yCount > 1);

    // by this point, we have strings x and y formed through a series of pairings of the initial strings 1 and 0
    // x is the final dominant pair and y is the second to last dominant pair
    Pattern pattern;
    for (int i = 0; i < xCount; i++) {
        pattern.append(x);
    }
    for (int i = 0; i < yCount; i++) {
        pattern.append(y);
    }

    return pattern;
}

} // namespace Rhythm
