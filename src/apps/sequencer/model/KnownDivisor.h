#pragma once

#include <cstdint>

// straight

// 3,      // 1/64
// 6,      // 1/32
// 12,     // 1/16
// 24,     // 1/8
// 48,     // 1/4
// 96,     // 1/2
// 192,    // 1

// triplet

// 2,      // 1/64T
// 4,      // 1/32T
// 8,      // 1/16T
// 16,     // 1/8T
// 32,     // 1/4T
// 64,     // 1/2T
// 128,    // 1T

// dotted

// 9,      // 1/32.
// 18,     // 1/16.
// 36,     // 1/8.
// 72,     // 1/4.
// 144,    // 1/2.

struct KnownDivisor {
    uint8_t divisor;        // divisor value with respect to CONFIG_SEQUENCE_PPQN
    uint8_t denominator;    // note value denominator (1/x)
    char type;              // note type (straight, triplet, dotted)
    int8_t index;           // index [0..15] for selecting via step keys
};

// divisors based on 48 PPQN

static KnownDivisor knownDivisors[] = {
    {   2,      64,     'T',    8   },  // 1/64T
    {   3,      64,     '\0',   0   },  // 1/64
    {   4,      32,     'T',    9   },  // 1/32T
    {   6,      32,     '\0',   1   },  // 1/32
    {   8,      16,     'T',    10  },  // 1/16T
    {   9,      32,     '.',    -1  },  // 1/32.
    {   12,     16,     '\0',   2   },  // 1/16
    {   16,     8,      'T',    11  },  // 1/8T
    {   18,     16,     '.',    -1  },  // 1/16.
    {   24,     8,      '\0',   3   },  // 1/8
    {   32,     4,      'T',    12  },  // 1/4T
    {   36,     8,      '.',    -1  },  // 1/8.
    {   48,     4,      '\0',   4   },  // 1/4
    {   64,     2,      'T',    13  },  // 1/2T
    {   72,     4,      '.',    -1  },  // 1/4.
    {   96,     2,      '\0',   5   },  // 1/2
    {   128,    1,      'T',    14  },  // 1T
    {   144,    2,      '.',    -1  },  // 1/2.
    {   192,    1,      '\0',   6   },  // 1
};

static const int numKnownDivisors = sizeof(knownDivisors) / sizeof(KnownDivisor);
