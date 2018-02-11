#pragma once

#include <algorithm>
#include <type_traits>

#include <cstdint>
#include <cstddef>

template<int Bits_>
struct UnsignedValue {
    static constexpr int Bits = Bits_;
    static constexpr int Min = 0;
    static constexpr int Max = (1 << Bits) - 1;
    static constexpr int Range = (1 << Bits);

    static int bits() { return Bits; }
    static int min() { return Min; }
    static int max() { return Max; }
    static int range() { return Range; }

    static int clamp(int value) {
        return std::max(min(), std::min(max(), value));
    }
};

template<int Bits_>
struct SignedValue {
    static constexpr int Bits = Bits_;
    static constexpr int Min = -(1 << (Bits - 1)) + 1;
    static constexpr int Max = (1 << (Bits - 1)) - 1;
    static constexpr int Range = (1 << Bits);

    static int bits() { return Bits; }
    static int min() { return Min; }
    static int max() { return Max; }
    static int range() { return Range; }

    static int clamp(int value) {
        return std::max(min(), std::min(max(), value));
    }
};

// the following is based on https://blog.codef00.com/2014/12/06/portable-bitfields-using-c11/

namespace detail {

    template <size_t LastBit>
    struct MinimumTypeHelper {
        typedef
            typename std::conditional<LastBit == 0 , void,
            typename std::conditional<LastBit <= 8 , uint8_t,
            typename std::conditional<LastBit <= 16, uint16_t,
            typename std::conditional<LastBit <= 32, uint32_t,
            typename std::conditional<LastBit <= 64, uint64_t,
            void>::type>::type>::type>::type>::type type;
    };

} // namespace detail

template <size_t Index, size_t Bits = 1>
class BitField {
private:
    enum {
        Mask = (1u << Bits) - 1u
    };

    typedef typename detail::MinimumTypeHelper<Index + Bits>::type T;
public:
    template <class T2>
    BitField &operator=(T2 value) {
        value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
        return *this;
    }

    operator T() const             { return (value_ >> Index) & Mask; }
    explicit operator bool() const { return value_ & (Mask << Index); }
    BitField &operator++()         { return *this = *this + 1; }
    T operator++(int)              { T r = *this; ++*this; return r; }
    BitField &operator--()         { return *this = *this - 1; }
    T operator--(int)              { T r = *this; --*this; return r; }

private:
    T value_;
};


template <size_t Index>
class BitField<Index, 1> {
private:
    enum {
        Bits = 1,
        Mask = 0x01
    };

    typedef typename detail::MinimumTypeHelper<Index + Bits>::type T;
public:
    BitField &operator=(bool value) {
        value_ = (value_ & ~(Mask << Index)) | (value << Index);
        return *this;
    }

    explicit operator bool() const { return value_ & (Mask << Index); }

private:
    T value_;
};
