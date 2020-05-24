#ifndef LIBCHESS_CASTLINGRIGHTS_H
#define LIBCHESS_CASTLINGRIGHTS_H

#include <cstdint>
#include <initializer_list>
#include <ostream>

#include "internal/MetaValueType.h"

namespace libchess {

class CastlingRight : public MetaValueType<int> {
   public:
    class Value {
       public:
        enum CastlingRightValue : value_type
        {
            CASTLING_RIGHT_NONE = 0,
            WHITE_KINGSIDE = 1,
            WHITE_QUEENSIDE = 2,
            BLACK_KINGSIDE = 4,
            BLACK_QUEENSIDE = 8
        };
    };
    constexpr explicit CastlingRight(value_type value) : MetaValueType<value_type>(value) {
    }

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

class CastlingRights {
   public:
    using value_type = int;

    constexpr CastlingRights() : CastlingRights(CastlingRight::Value::CASTLING_RIGHT_NONE) {
    }
    constexpr explicit CastlingRights(value_type value) : value_(value) {
    }
    constexpr explicit CastlingRights(CastlingRight castling_right)
        : value_(castling_right.value()) {
    }
    constexpr CastlingRights(std::initializer_list<CastlingRight> castling_right_list) : value_(0) {
        for (auto castling_right : castling_right_list) {
            allow(castling_right);
        }
    }

    constexpr bool operator==(CastlingRights rhs) const {
        return value() == rhs.value();
    }

    constexpr void allow(CastlingRight castling_right) {
        value_ |= value_type(castling_right.value());
    }
    constexpr void disallow(CastlingRight castling_right) {
        value_ &= ~value_type(castling_right.value());
    }

    constexpr bool is_allowed(CastlingRight castling_right) const {
        return value_ & value_type(castling_right.value());
    }

    static CastlingRights from(const std::string& str) {
        CastlingRights castling_rights;
        for (char c : str) {
            castling_rights.allow(CastlingRight::from(c));
        }
        return castling_rights;
    }
    std::string to_str() const {
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

    constexpr value_type value() const {
        return value_;
    }
    constexpr value_type& value_mut_ref() {
        return value_;
    }

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
constexpr static CastlingRights CASTLING_RIGHTS_ALL{WHITE_KINGSIDE,
                                                    WHITE_QUEENSIDE,
                                                    BLACK_KINGSIDE,
                                                    BLACK_QUEENSIDE};

}  // namespace constants

}  // namespace libchess

namespace std {

template <>
struct hash<libchess::CastlingRight> : public hash<libchess::CastlingRight::value_type> {};
template <>
struct hash<libchess::CastlingRights> : public hash<libchess::CastlingRights::value_type> {};

}  // namespace std

#endif  // LIBCHESS_CASTLINGRIGHTS_H
