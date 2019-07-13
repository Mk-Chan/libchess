#ifndef LIBCHESS_RANK_H
#define LIBCHESS_RANK_H

#include <cstdint>

#include "internal/MetaValueType.h"

namespace libchess {

/// Represents a rank on the chess board (horizontal/row).
class Rank : public MetaValueType<int> {
  public:
    /// Set of possible Rank values.
    class Value {
      public:
        enum RankValue : value_type {
            RANK_1,
            RANK_2,
            RANK_3,
            RANK_4,
            RANK_5,
            RANK_6,
            RANK_7,
            RANK_8
        };
    };
    constexpr explicit Rank(value_type value) noexcept : MetaValueType<value_type>(value) {}

    /// The character representation of a Rank.
    [[nodiscard]] constexpr char to_char() const noexcept { return '1' + value(); }

    /// Parses a Rank from a character.
    [[nodiscard]] constexpr static std::optional<Rank> from(char c) noexcept {
        int offset = c - '1';
        if (offset < 0 || offset > 7) {
            return std::nullopt;
        }
        return Rank{Value::RANK_1 + offset};
    }
};

namespace constants {

constexpr static Rank RANK_1 = Rank{Rank::Value::RANK_1};
constexpr static Rank RANK_2 = Rank{Rank::Value::RANK_2};
constexpr static Rank RANK_3 = Rank{Rank::Value::RANK_3};
constexpr static Rank RANK_4 = Rank{Rank::Value::RANK_4};
constexpr static Rank RANK_5 = Rank{Rank::Value::RANK_5};
constexpr static Rank RANK_6 = Rank{Rank::Value::RANK_6};
constexpr static Rank RANK_7 = Rank{Rank::Value::RANK_7};
constexpr static Rank RANK_8 = Rank{Rank::Value::RANK_8};

} // namespace constants

} // namespace libchess

namespace std {

template <> struct hash<libchess::Rank> : public hash<libchess::Rank::value_type> {};

} // namespace std

#endif // LIBCHESS_RANK_H
