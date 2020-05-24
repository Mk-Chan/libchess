#ifndef LIBCHESS_BITBOARD_H
#define LIBCHESS_BITBOARD_H

#include <cstdint>
#include <iostream>
#include <optional>

#include "Square.h"

namespace libchess {

class Bitboard {
   public:
    using value_type = std::uint64_t;

    constexpr Bitboard() : value_(0) {
    }
    constexpr explicit Bitboard(int shift) : value_(std::uint64_t(1) << shift) {
    }
    constexpr explicit Bitboard(unsigned shift) : value_(std::uint64_t(1) << shift) {
    }
    constexpr explicit Bitboard(value_type value) : value_(value) {
    }
    constexpr explicit Bitboard(Square square) : Bitboard(square.value()) {
    }

    constexpr operator value_type() const {
        return value_;
    }

    constexpr Bitboard operator<<(int shift) const {
        return Bitboard{value_ << unsigned(shift)};
    }
    constexpr Bitboard operator<<(unsigned shift) const {
        return Bitboard{value_ << shift};
    }
    constexpr Bitboard operator>>(int shift) const {
        return Bitboard{value_ >> unsigned(shift)};
    }
    constexpr Bitboard operator>>(unsigned shift) const {
        return Bitboard{value_ >> shift};
    }
    constexpr Bitboard operator|(value_type rhs) const {
        return Bitboard{value_ | rhs};
    }
    constexpr Bitboard operator&(value_type rhs) const {
        return Bitboard{value_ & rhs};
    }
    constexpr Bitboard operator^(value_type rhs) const {
        return Bitboard{value_ ^ rhs};
    }
    constexpr Bitboard operator|(Bitboard rhs) const {
        return Bitboard{value_ | rhs};
    }
    constexpr Bitboard operator&(Bitboard rhs) const {
        return Bitboard{value_ & rhs};
    }
    constexpr Bitboard operator^(Bitboard rhs) const {
        return Bitboard{value_ ^ rhs};
    }

    constexpr Bitboard operator~() const {
        return Bitboard{~value_};
    }
    constexpr bool operator!() const {
        return value_ == 0;
    }
    constexpr Bitboard operator-() const {
        return Bitboard{~value_};
    }
    constexpr Bitboard& operator<<=(int shift) {
        value_ <<= unsigned(shift);
        return *this;
    }
    constexpr Bitboard& operator<<=(unsigned shift) {
        value_ <<= shift;
        return *this;
    }
    constexpr Bitboard& operator>>=(int shift) {
        value_ >>= unsigned(shift);
        return *this;
    }
    constexpr Bitboard& operator>>=(unsigned shift) {
        value_ >>= shift;
        return *this;
    }
    constexpr Bitboard& operator|=(value_type rhs) {
        value_ |= rhs;
        return *this;
    }
    constexpr Bitboard& operator&=(value_type rhs) {
        value_ &= rhs;
        return *this;
    }
    constexpr Bitboard& operator^=(value_type rhs) {
        value_ ^= rhs;
        return *this;
    }
    constexpr int popcount() const {
        return __builtin_popcountll(value_);
    }
    constexpr Square forward_bitscan() const {
        return Square{__builtin_ctzll(value_)};
    }
    constexpr Square reverse_bitscan() const {
        return Square{63 - __builtin_clzll(value_)};
    }
    constexpr void forward_popbit() {
        value_ &= value_ - 1;
    }
    constexpr void reverse_popbit() {
        value_ ^= Bitboard{reverse_bitscan() + 1};
    }

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

}  // namespace libchess

namespace std {

template <>
struct hash<libchess::Bitboard> : public hash<libchess::Bitboard::value_type> {};

}  // namespace std

#endif  // LIBCHESS_BITBOARD_H
