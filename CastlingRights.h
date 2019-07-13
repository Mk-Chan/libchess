#ifndef LIBCHESS_CASTLINGRIGHTS_H
#define LIBCHESS_CASTLINGRIGHTS_H

#include <cstdint>
#include <initializer_list>

#include "internal/MetaValueType.h"

namespace libchess {

/// Represents a single castling right {White Kingside, Black Queenside, ...}.
class CastlingRight : public MetaValueType<int> {
  public:
    /// Set of possible CastlingRight values.
    class Value {
      public:
        enum CastlingRightValue : value_type {
            CASTLING_RIGHT_NONE = 0,
            WHITE_KINGSIDE = 1,
            WHITE_QUEENSIDE = 2,
            BLACK_KINGSIDE = 4,
            BLACK_QUEENSIDE = 8
        };
    };
    constexpr explicit CastlingRight(value_type value) : MetaValueType<value_type>(value) {}

    /// Parses CastlingRight from a character {'K', 'Q', 'k', 'q'}.
    constexpr static CastlingRight from(char c) {
        switch (c) {
        case 'K':
            return CastlingRight{Value::WHITE_KINGSIDE};
        case 'Q':
            return CastlingRight{Value::WHITE_QUEENSIDE};
        case 'k':
            return CastlingRight{Value::BLACK_KINGSIDE};
        case 'q':
            return CastlingRight{Value::BLACK_QUEENSIDE};
        default:
            return CastlingRight{Value::CASTLING_RIGHT_NONE};
        }
    }
};

inline std::ostream& operator<<(std::ostream& ostream, CastlingRight castling_right) {
    char c = '-';
    switch (castling_right.value()) {
    case CastlingRight::Value::WHITE_KINGSIDE:
        c = 'K';
        break;
    case CastlingRight::Value::WHITE_QUEENSIDE:
        c = 'Q';
        break;
    case CastlingRight::Value::BLACK_KINGSIDE:
        c = 'k';
        break;
    case CastlingRight::Value::BLACK_QUEENSIDE:
        c = 'q';
        break;
    }
    return ostream << c;
}

/// Represents a complete set of `CastlingRight`s allowed to a position.
class CastlingRights {
  public:
    using value_type = int;

    constexpr CastlingRights() noexcept
        : CastlingRights(CastlingRight::Value::CASTLING_RIGHT_NONE) {}
    constexpr explicit CastlingRights(value_type value) noexcept : value_(value) {}
    constexpr explicit CastlingRights(CastlingRight castling_right) noexcept
        : value_(castling_right.value()) {}
    constexpr CastlingRights(std::initializer_list<CastlingRight> castling_right_list) noexcept
        : value_(0) {
        for (auto castling_right : castling_right_list) {
            allow(castling_right);
        }
    }

    [[nodiscard]] constexpr bool operator==(CastlingRights rhs) const noexcept {
        return value() == rhs.value();
    }

    /// Adds `castling_right` to the set of allowed `CastlingRight`s.
    constexpr void allow(CastlingRight castling_right) {
        value_ |= value_type(castling_right.value());
    }

    /// Removes `castling_right` from the set of allowed `CastlingRight`s.
    constexpr void disallow(CastlingRight castling_right) {
        value_ &= ~value_type(castling_right.value());
    }

    /// Checks if `castling_right` is in the set of allowed `CastlingRight`s.
    [[nodiscard]] constexpr bool is_allowed(CastlingRight castling_right) const noexcept {
        return value_ & value_type(castling_right.value());
    }

    /// Parses CastlingRights from a string.
    [[nodiscard]] static CastlingRights from(const std::string& str) noexcept {
        CastlingRights castling_rights;
        for (char c : str) {
            castling_rights.allow(CastlingRight::from(c));
        }
        return castling_rights;
    }

    /// The string representation of CastlingRights.
    [[nodiscard]] std::string to_str() const noexcept {
        std::string cr_str;
        bool any = false;
        if (is_allowed(CastlingRight{CastlingRight::Value::WHITE_KINGSIDE})) {
            cr_str += "K";
            any = true;
        }
        if (is_allowed(CastlingRight{CastlingRight::Value::WHITE_QUEENSIDE})) {
            cr_str += "Q";
            any = true;
        }
        if (is_allowed(CastlingRight{CastlingRight::Value::BLACK_KINGSIDE})) {
            cr_str += "k";
            any = true;
        }
        if (is_allowed(CastlingRight{CastlingRight::Value::BLACK_QUEENSIDE})) {
            cr_str += "q";
            any = true;
        }
        if (!any) {
            cr_str += "-";
        }
        return cr_str;
    }

    [[nodiscard]] constexpr value_type value() const noexcept { return value_; }

  private:
    value_type value_;
};

inline std::ostream& operator<<(std::ostream& ostream, CastlingRights castling_rights) {
    return ostream << castling_rights.to_str();
}

namespace constants {

constexpr static CastlingRight CASTLING_RIGHT_NONE{CastlingRight::Value::CASTLING_RIGHT_NONE};
constexpr static CastlingRight WHITE_KINGSIDE{CastlingRight::Value::WHITE_KINGSIDE};
constexpr static CastlingRight WHITE_QUEENSIDE{CastlingRight::Value::WHITE_QUEENSIDE};
constexpr static CastlingRight BLACK_KINGSIDE{CastlingRight::Value::BLACK_KINGSIDE};
constexpr static CastlingRight BLACK_QUEENSIDE{CastlingRight::Value::BLACK_QUEENSIDE};

constexpr static CastlingRights CASTLING_RIGHTS_NONE{};
constexpr static CastlingRights CASTLING_RIGHTS_ALL{WHITE_KINGSIDE, WHITE_QUEENSIDE, BLACK_KINGSIDE,
                                                    BLACK_QUEENSIDE};

} // namespace constants

} // namespace libchess

namespace std {

template <>
struct hash<libchess::CastlingRight> : public hash<libchess::CastlingRight::value_type> {};
template <>
struct hash<libchess::CastlingRights> : public hash<libchess::CastlingRights::value_type> {};

} // namespace std

#endif // LIBCHESS_CASTLINGRIGHTS_H
