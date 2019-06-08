#ifndef LIBCHESS_RANK_H
#define LIBCHESS_RANK_H

#include <cstdint>

#include "internal/MetaValueType.h"

namespace libchess {

class Rank : public MetaValueType<std::uint8_t> {
  public:
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
    constexpr inline Rank(value_type value) : MetaValueType<value_type>(value) {}

    constexpr inline char to_char() const { return '1' + value(); }

    constexpr static inline Rank from(char c) {
        return Value::RANK_1 + (c - '1');
    }
};

namespace constants {

constexpr inline Rank RANK_1 = Rank{Rank::Value::RANK_1};
constexpr inline Rank RANK_2 = Rank{Rank::Value::RANK_2};
constexpr inline Rank RANK_3 = Rank{Rank::Value::RANK_3};
constexpr inline Rank RANK_4 = Rank{Rank::Value::RANK_4};
constexpr inline Rank RANK_5 = Rank{Rank::Value::RANK_5};
constexpr inline Rank RANK_6 = Rank{Rank::Value::RANK_6};
constexpr inline Rank RANK_7 = Rank{Rank::Value::RANK_7};
constexpr inline Rank RANK_8 = Rank{Rank::Value::RANK_8};

} // namespace constants

} // namespace libchess

#endif // LIBCHESS_RANK_H
