#ifndef LIBCHESS_PIECETYPE_H
#define LIBCHESS_PIECETYPE_H

#include <array>

#include "internal/MetaValueType.h"

namespace libchess {

/// Represents a type of a chess piece {pawn, knight, ...}.
class PieceType : public MetaValueType<int> {
  public:
    /// Set of possible PieceType values.
    class Value {
      public:
        enum PieceTypeValue : value_type {
            PAWN,
            KNIGHT,
            BISHOP,
            ROOK,
            QUEEN,
            KING,
        };
    };
    constexpr explicit PieceType(value_type value) noexcept : MetaValueType<value_type>(value) {}

    /// The character representation of the PieceType.
    [[nodiscard]] constexpr char to_char() const noexcept {
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

    /// Parses a PieceType from a character.
    [[nodiscard]] constexpr static std::optional<PieceType> from(char c) noexcept {
        switch (c) {
        case 'p':
        case 'P':
            return PieceType{Value::PAWN};
        case 'n':
        case 'N':
            return PieceType{Value::KNIGHT};
        case 'b':
        case 'B':
            return PieceType{Value::BISHOP};
        case 'r':
        case 'R':
            return PieceType{Value::ROOK};
        case 'q':
        case 'Q':
            return PieceType{Value::QUEEN};
        case 'k':
        case 'K':
            return PieceType{Value::KING};
        default:
            return {};
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
constexpr static std::array<PieceType, 6> PIECE_TYPES{PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};

} // namespace constants

} // namespace libchess

namespace std {

template <> struct hash<libchess::PieceType> : public hash<libchess::PieceType::value_type> {};

} // namespace std

#endif // LIBCHESS_PIECETYPE_H
