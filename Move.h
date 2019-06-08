#ifndef LIBCHESS_MOVE_H
#define LIBCHESS_MOVE_H

#include <algorithm>

#include "PieceType.h"
#include "Square.h"

namespace libchess {

enum class MoveValues : std::uint32_t { MOVE_INVALID };

class Move {
  private:
    enum BitOpLookup : std::uint32_t {
        TO_SQUARE_SHIFT = 6,
        MOVE_TYPE_SHIFT = 12,
        PROMOTION_TYPE_SHIFT = 15,

        MOVE_TYPE_MASK = 7 << MOVE_TYPE_SHIFT,
        PROMOTION_TYPE_MASK = 7 << PROMOTION_TYPE_SHIFT
    };
    constexpr inline Move(std::uint32_t value) : value_(value) {}

  public:
    enum class Type : std::uint8_t {
        NORMAL,
        CASTLING,
        ENPASSANT,
        PROMOTION,
        DOUBLE_PUSH,
        CAPTURE,
        CAPTURE_PROMOTION,
        NONE
    };

    using value_type = std::uint32_t;

    constexpr inline Move() : value_(static_cast<value_type>(MoveValues::MOVE_INVALID)) {}
    constexpr inline Move(Square from_square, Square to_square,
                          PieceType promotion_pt = constants::PIECE_TYPE_NONE,
                          Move::Type type = Move::Type::NONE)
        : value_(from_square.value() | (to_square.value() << TO_SQUARE_SHIFT) |
                 (promotion_pt.value() << PROMOTION_TYPE_SHIFT) |
                 (std::uint32_t(type) << MOVE_TYPE_SHIFT)) {}

    constexpr inline bool operator==(Move rhs) const { return value() == rhs.value(); }

    constexpr inline Square from_square() const { return value() & 0x3f; }
    constexpr inline Square to_square() const { return (value() & 0xfc0) >> 6; }
    constexpr inline Move::Type type() const {
        return static_cast<Move::Type>((value() & MOVE_TYPE_MASK) >> MOVE_TYPE_SHIFT);
    }
    constexpr inline PieceType promotion_piece_type() const {
        return (value() & PROMOTION_TYPE_MASK) >> PROMOTION_TYPE_SHIFT;
    }

    constexpr inline value_type value() const { return value_; }

  private:
    value_type value_;
};

class MoveList {
  private:
    constexpr static inline int max_size = 256;

  public:
    using iterator = std::array<Move, max_size>::iterator;
    using const_iterator = std::array<Move, max_size>::const_iterator;

    constexpr inline iterator begin() { return values_.begin(); }
    constexpr inline iterator end() { return values_.begin() + size(); }
    constexpr inline const_iterator cbegin() const { return values_.begin(); }
    constexpr inline const_iterator cend() const { return values_.begin() + size(); }

    constexpr inline void add(Move move) {
        values_[size()] = move;
        ++size_;
    }
    template <class F> constexpr inline void sort(F comparator) {
        std::sort(begin(), end(), comparator);
    }
    constexpr inline int size() const { return size_; }

  protected:
    constexpr inline const std::array<Move, max_size>& values() const { return values_; }

  private:
    int size_ = 0;
    std::array<Move, max_size> values_;
};

inline std::ostream& operator<<(std::ostream& ostream, Move move) {
    ostream << move.from_square() << move.to_square();
    if (move.promotion_piece_type() != constants::PIECE_TYPE_NONE) {
        ostream << move.promotion_piece_type();
    }
    return ostream;
}

namespace constants {

constexpr inline Move::Type MOVE_TYPE_NONE = Move::Type::NONE;

} // namespace constants

} // namespace libchess

#endif // LIBCHESS_MOVE_H
