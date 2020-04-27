#pragma once

#define ENUM_CLASS_OPERATORS(e_) \
    inline e_ operator&(e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a) & static_cast<int>(b)); } \
    inline e_ operator|(e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a) | static_cast<int>(b)); } \
    inline e_ &operator|=(e_& a, e_ b) { a = a | b; return a; }; \
    inline e_ &operator&=(e_& a, e_ b) { a = a & b; return a; }; \
    inline e_ operator~(e_ a) { return static_cast<e_>(~static_cast<int>(a)); }
