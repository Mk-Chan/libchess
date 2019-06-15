#ifndef LIBCHESS_PIECETYPE_H
#define LIBCHESS_PIECETYPE_H

#include "internal/MetaValueType.h"

namespace libchess {

class PieceType : public MetaValueType<int> {
  public:
    class Value {
      public:
        enum PieceTypeValue : value_type {
            PAWN,
            KNIGHT,
            BISHOP,
            ROOK,
            QUEEN,
            KING,
            NUM_PIECE_TYPES,
            NONE
        };
    };
    constexpr inline PieceType(value_type value) : MetaValueType<value_type>(value) {}
    constexpr inline PieceType() : PieceType(Value::NONE) {}

    constexpr inline char to_char() const {
        switch (value()) {
        case Value::PAWN:
            return 'p';
        case Value::KNIGHT:
            return 'n';
        case Value::BISHOP:
            return 'b';
        case Value::ROOK:
            return 'r';
        case Value::QUEEN:
            return 'q';
        case Value::KING:
            return 'k';
        default:
            return '-';
        }
    }

    constexpr inline static PieceType from(char c) {
        switch (c) {
        case 'p':
        case 'P':
            return Value::PAWN;
        case 'n':
        case 'N':
            return Value::KNIGHT;
        case 'b':
        case 'B':
            return Value::BISHOP;
        case 'r':
        case 'R':
            return Value::ROOK;
        case 'q':
        case 'Q':
            return Value::QUEEN;
        case 'k':
        case 'K':
            return Value::KING;
        default:
            return Value::NONE;
        }
    }
};

inline std::ostream& operator<<(std::ostream& ostream, PieceType piece_type) {
    return ostream << piece_type.to_char();
}

namespace constants {

constexpr static PieceType PAWN{PieceType::Value::PAWN};
constexpr static PieceType KNIGHT{PieceType::Value::KNIGHT};
constexpr static PieceType BISHOP{PieceType::Value::BISHOP};
constexpr static PieceType ROOK{PieceType::Value::ROOK};
constexpr static PieceType QUEEN{PieceType::Value::QUEEN};
constexpr static PieceType KING{PieceType::Value::KING};
constexpr static PieceType PIECE_TYPE_NONE{PieceType::Value::NONE};
constexpr static PieceType PIECE_TYPES[]{PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};
constexpr static int NUM_PIECE_TYPES = PieceType::Value::NUM_PIECE_TYPES;

} // namespace constants

} // namespace libchess

namespace std {

template <> struct hash<libchess::PieceType> : public hash<libchess::PieceType::value_type> {};

} // namespace std

#endif // LIBCHESS_PIECETYPE_H
