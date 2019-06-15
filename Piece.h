#ifndef LIBCHESS_PIECE_H
#define LIBCHESS_PIECE_H

#include "Color.h"
#include "PieceType.h"

namespace libchess {

class Piece {
  public:
    using value_type = int;

    constexpr inline Piece(PieceType piece_type, Color piece_color)
        : value_(piece_type.value() | (piece_color.value() << 3)) {}

    constexpr inline PieceType type() const { return value_ & 7; }
    constexpr inline Color color() const { return value_ >> 3; }

    constexpr inline bool operator==(const Piece rhs) const {
        return type() == rhs.type() && color() == rhs.color();
    }
    constexpr inline bool operator!=(const Piece rhs) const {
        return type() != rhs.type() || color() != rhs.color();
    }

    constexpr inline char to_char() const {
        char piece_type_char = type().to_char();
        if (color() == constants::WHITE) {
            piece_type_char = std::toupper(piece_type_char);
        } else {
            piece_type_char = std::tolower(piece_type_char);
        }
        return piece_type_char;
    }

    constexpr inline static Piece from(Color piece_color, PieceType piece_type) {
        return Piece{piece_type, piece_color};
    }
    constexpr inline static Piece from(char c) {
        PieceType piece_type = PieceType::from(c);
        Color piece_color = c >= 'a' ? constants::BLACK : constants::WHITE;
        if (piece_type == constants::PIECE_TYPE_NONE) {
            piece_color = constants::COLOR_NONE;
        }
        return Piece::from(piece_color, piece_type);
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

constexpr static Piece PIECE_NONE = Piece{PIECE_TYPE_NONE, COLOR_NONE};
constexpr static Piece WHITE_PAWN = Piece::from('P');
constexpr static Piece WHITE_KNIGHT = Piece::from('N');
constexpr static Piece WHITE_BISHOP = Piece::from('B');
constexpr static Piece WHITE_ROOK = Piece::from('R');
constexpr static Piece WHITE_QUEEN = Piece::from('Q');
constexpr static Piece WHITE_KING = Piece::from('K');
constexpr static Piece BLACK_PAWN = Piece::from('p');
constexpr static Piece BLACK_KNIGHT = Piece::from('n');
constexpr static Piece BLACK_BISHOP = Piece::from('b');
constexpr static Piece BLACK_ROOK = Piece::from('r');
constexpr static Piece BLACK_QUEEN = Piece::from('q');
constexpr static Piece BLACK_KING = Piece::from('k');
constexpr static Piece PIECES[]{BLACK_PAWN,   WHITE_PAWN,   BLACK_KNIGHT, WHITE_KNIGHT,
                                BLACK_BISHOP, WHITE_BISHOP, BLACK_ROOK,   WHITE_ROOK,
                                BLACK_QUEEN,  WHITE_QUEEN,  BLACK_KING,   WHITE_KING};

} // namespace constants

} // namespace libchess

namespace std {

template <> struct hash<libchess::Piece> : public hash<libchess::Piece::value_type> {};

} // namespace std

#endif // LIBCHESS_PIECE_H
