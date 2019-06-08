#ifndef LIBCHESS_BITBOARD_H
#define LIBCHESS_BITBOARD_H

#include <cstdint>
#include <iostream>

#include "Square.h"

namespace libchess {

class Bitboard {
  public:
    using value_type = std::uint64_t;

    constexpr inline Bitboard() : value_(0) {}
    constexpr inline Bitboard(int shift) : value_(std::uint64_t(1) << shift) {}
    constexpr inline Bitboard(unsigned shift) : value_(std::uint64_t(1) << shift) {}
    constexpr inline Bitboard(value_type value) : value_(value) {}
    constexpr inline Bitboard(Square square) : Bitboard(square.value()) {}

    constexpr inline operator value_type() const { return value_; }

    constexpr inline Bitboard operator<<(int shift) const { return value_ << unsigned(shift); }
    constexpr inline Bitboard operator<<(unsigned shift) const { return value_ << shift; }
    constexpr inline Bitboard operator>>(int shift) const { return value_ >> unsigned(shift); }
    constexpr inline Bitboard operator>>(unsigned shift) const { return value_ >> shift; }
    constexpr inline Bitboard operator|(value_type rhs) const { return value_ | rhs; }
    constexpr inline Bitboard operator&(value_type rhs) const { return value_ & rhs; }
    constexpr inline Bitboard operator^(value_type rhs) const { return value_ ^ rhs; }

    constexpr inline Bitboard operator~() const { return ~value_; }
    constexpr inline bool operator!() const { return value_ == 0; }
    constexpr inline Bitboard operator-() const { return ~value_; }
    constexpr inline Bitboard& operator<<=(int shift) {
        value_ <<= unsigned(shift);
        return *this;
    }
    constexpr inline Bitboard& operator<<=(unsigned shift) {
        value_ <<= shift;
        return *this;
    }
    constexpr inline Bitboard& operator>>=(int shift) {
        value_ >>= unsigned(shift);
        return *this;
    }
    constexpr inline Bitboard& operator>>=(unsigned shift) {
        value_ >>= shift;
        return *this;
    }
    constexpr inline Bitboard& operator|=(value_type rhs) {
        value_ |= rhs;
        return *this;
    }
    constexpr inline Bitboard& operator&=(value_type rhs) {
        value_ &= rhs;
        return *this;
    }
    constexpr inline Bitboard& operator^=(value_type rhs) {
        value_ ^= rhs;
        return *this;
    }
    constexpr inline Square forward_bitscan() const { return __builtin_ctzll(value_); }
    constexpr inline Square reverse_bitscan() const { return 63 - __builtin_clzll(value_); }
    constexpr inline void forward_popbit() { value_ &= value_ - 1; }
    constexpr inline void reverse_popbit() { value_ ^= Bitboard{reverse_bitscan() + 1}; }

  private:
    value_type value_;
};

inline std::ostream& operator<<(std::ostream& ostream, Bitboard bb) {
    for (unsigned sq = 0; sq < 64; ++sq) {
        if (sq && !(sq & 7u)) {
            ostream << "\n";
        }

        if (Bitboard(sq ^ 56u) & bb) {
            ostream << "X  ";
        } else {
            ostream << "-  ";
        }
    }
    ostream << "\n";
    return ostream;
}

} // namespace libchess

#endif // LIBCHESS_BITBOARD_H
