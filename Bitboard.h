#ifndef LIBCHESS_BITBOARD_H
#define LIBCHESS_BITBOARD_H

#include <cstdint>
#include <iostream>

#include "Square.h"

namespace libchess {

/// Represents some type of 64-square chess board occupancy.
class Bitboard {
  public:
    using value_type = std::uint64_t;

    constexpr Bitboard() noexcept : value_(0) {}
    constexpr explicit Bitboard(int shift) noexcept : value_(std::uint64_t(1) << shift) {}
    constexpr explicit Bitboard(unsigned shift) noexcept : value_(std::uint64_t(1) << shift) {}
    constexpr explicit Bitboard(value_type value) noexcept : value_(value) {}
    constexpr explicit Bitboard(Square square) noexcept : Bitboard(square.value()) {}

    constexpr operator value_type() const noexcept { return value_; }

    constexpr Bitboard operator<<(int shift) const noexcept {
        return Bitboard{value_ << unsigned(shift)};
    }
    constexpr Bitboard operator<<(unsigned shift) const noexcept {
        return Bitboard{value_ << shift};
    }
    constexpr Bitboard operator>>(int shift) const noexcept {
        return Bitboard{value_ >> unsigned(shift)};
    }
    constexpr Bitboard operator>>(unsigned shift) const noexcept {
        return Bitboard{value_ >> shift};
    }
    constexpr Bitboard operator|(value_type rhs) const noexcept { return Bitboard{value_ | rhs}; }
    constexpr Bitboard operator&(value_type rhs) const noexcept { return Bitboard{value_ & rhs}; }
    constexpr Bitboard operator^(value_type rhs) const noexcept { return Bitboard{value_ ^ rhs}; }
    constexpr Bitboard operator|(Bitboard rhs) const noexcept { return Bitboard{value_ | rhs}; }
    constexpr Bitboard operator&(Bitboard rhs) const noexcept { return Bitboard{value_ & rhs}; }
    constexpr Bitboard operator^(Bitboard rhs) const noexcept { return Bitboard{value_ ^ rhs}; }

    constexpr Bitboard operator~() const noexcept { return Bitboard{~value_}; }
    constexpr bool operator!() const noexcept { return value_ == 0; }
    constexpr Bitboard operator-() const noexcept { return Bitboard{~value_}; }
    constexpr Bitboard& operator<<=(int shift) noexcept {
        value_ <<= unsigned(shift);
        return *this;
    }
    constexpr Bitboard& operator<<=(unsigned shift) noexcept {
        value_ <<= shift;
        return *this;
    }
    constexpr Bitboard& operator>>=(int shift) noexcept {
        value_ >>= unsigned(shift);
        return *this;
    }
    constexpr Bitboard& operator>>=(unsigned shift) noexcept {
        value_ >>= shift;
        return *this;
    }
    constexpr Bitboard& operator|=(value_type rhs) noexcept {
        value_ |= rhs;
        return *this;
    }
    constexpr Bitboard& operator&=(value_type rhs) noexcept {
        value_ &= rhs;
        return *this;
    }
    constexpr Bitboard& operator^=(value_type rhs) noexcept {
        value_ ^= rhs;
        return *this;
    }

    /// The number of set bits in the Bitboard.
    [[nodiscard]] constexpr int popcount() const noexcept { return __builtin_popcountll(value_); }

    /// The first Square representative of the first set bit in the Bitboard.
    [[nodiscard]] constexpr Square forward_bitscan() const noexcept {
        return Square{__builtin_ctzll(value_)};
    }

    /// The last Square representative of the last set bit in the Bitboard.
    [[nodiscard]] constexpr Square reverse_bitscan() const noexcept {
        return Square{63 - __builtin_clzll(value_)};
    }

    /// Unsets the first set bit in the Bitboard.
    constexpr void forward_popbit() noexcept { value_ &= value_ - 1; }

    /// Unsets the last set bit in the Bitboard.
    constexpr void reverse_popbit() noexcept { value_ ^= Bitboard{reverse_bitscan() + 1}; }

  private:
    value_type value_;
};

inline std::ostream& operator<<(std::ostream& ostream, Bitboard bb) {
    for (unsigned sq = 0; sq < 64; ++sq) {
        if (sq && !(sq & 7U)) {
            ostream << "\n";
        }

        if (Bitboard(sq ^ 56U) & bb) {
            ostream << "X  ";
        } else {
            ostream << "-  ";
        }
    }
    ostream << "\n";
    return ostream;
}

} // namespace libchess

namespace std {

template <> struct hash<libchess::Bitboard> : public hash<libchess::Bitboard::value_type> {};

} // namespace std

#endif // LIBCHESS_BITBOARD_H
