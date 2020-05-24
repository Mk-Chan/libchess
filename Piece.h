#ifndef LIBCHESS_PIECE_H
#define LIBCHESS_PIECE_H

#include "Color.h"
#include "PieceType.h"

namespace libchess {

class Piece {
   public:
    using value_type = int;

    constexpr Piece(PieceType piece_type, Color piece_color)
        : value_(piece_type.value() | (piece_color.value() << 3)) {
    }

    constexpr PieceType type() const {
        return PieceType{value_ & 7};
    }
    constexpr Color color() const {
        return Color{value_ >> 3};
    }

    constexpr bool operator==(const Piece rhs) const {
        return type() == rhs.type() && color() == rhs.color();
    }
    constexpr bool operator!=(const Piece rhs) const {
        return type() != rhs.type() || color() != rhs.color();
    }

    constexpr char to_char() const {
        char piece_type_char = type().to_char();
        if (color() == constants::WHITE) {
            piece_type_char = std::toupper(piece_type_char);
        } else {
            piece_type_char = std::tolower(piece_type_char);
        }
        return piece_type_char;
    }

    constexpr static std::optional<Piece> from(std::optional<PieceType> piece_type,
                                               std::optional<Color> piece_color) {
        if (!(piece_type && piece_color)) {
            return {};
        }
        return Piece{*piece_type, *piece_color};
    }
    constexpr static std::optional<Piece> from(char c) {
        auto piece_type = PieceType::from(c);
        if (!piece_type) {
            return {};
        }

        Color piece_color = std::isupper(c) ? constants::WHITE : constants::BLACK;
        return Piece::from(*piece_type, piece_color);
    }

   private:
    value_type value_;
};

inline std::ostream& operator<<(std::ostream& ostream, Piece& piece) {
    Color color = piece.color();
    char piece_type_char = piece.type().to_char();
    if (color == constants::WHITE) {
        piece_type_char = std::toupper(piece_type_char);
    }
    return ostream << piece_type_char;
}

namespace constants {

constexpr static Piece WHITE_PAWN = *Piece::from(PAWN, WHITE);
constexpr static Piece WHITE_KNIGHT = *Piece::from(KNIGHT, WHITE);
constexpr static Piece WHITE_BISHOP = *Piece::from(BISHOP, WHITE);
constexpr static Piece WHITE_ROOK = *Piece::from(ROOK, WHITE);
constexpr static Piece WHITE_QUEEN = *Piece::from(QUEEN, WHITE);
constexpr static Piece WHITE_KING = *Piece::from(KING, WHITE);
constexpr static Piece BLACK_PAWN = *Piece::from(PAWN, BLACK);
constexpr static Piece BLACK_KNIGHT = *Piece::from(KNIGHT, BLACK);
constexpr static Piece BLACK_BISHOP = *Piece::from(BISHOP, BLACK);
constexpr static Piece BLACK_ROOK = *Piece::from(ROOK, BLACK);
constexpr static Piece BLACK_QUEEN = *Piece::from(QUEEN, BLACK);
constexpr static Piece BLACK_KING = *Piece::from(KING, BLACK);
constexpr static Piece PIECES[]{BLACK_PAWN,
                                WHITE_PAWN,
                                BLACK_KNIGHT,
                                WHITE_KNIGHT,
                                BLACK_BISHOP,
                                WHITE_BISHOP,
                                BLACK_ROOK,
                                WHITE_ROOK,
                                BLACK_QUEEN,
                                WHITE_QUEEN,
                                BLACK_KING,
                                WHITE_KING};

}  // namespace constants

}  // namespace libchess

namespace std {

template <>
struct hash<libchess::Piece> : public hash<libchess::Piece::value_type> {};

}  // namespace std

#endif  // LIBCHESS_PIECE_H
