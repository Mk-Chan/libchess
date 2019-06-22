#ifndef LIBCHESS_MOVE_H
#define LIBCHESS_MOVE_H

#include <algorithm>
#include <optional>
#include <vector>

#include "PieceType.h"
#include "Square.h"

namespace libchess {

class Move {
  private:
    enum BitOpLookup : std::uint32_t {
        TO_SQUARE_SHIFT = 6,
        PROMOTION_TYPE_SHIFT = 12,
        MOVE_TYPE_SHIFT = 15,

        PROMOTION_TYPE_MASK = 7 << PROMOTION_TYPE_SHIFT,
        MOVE_TYPE_MASK = 7 << MOVE_TYPE_SHIFT
    };
    enum class PromotionPieceType : int {
        NONE,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
    };

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

    constexpr inline Move(std::uint32_t value) : value_(value) {}
    constexpr inline Move(Square from_square, Square to_square, Move::Type type = Move::Type::NONE)
        : value_(from_square.value() | (to_square.value() << TO_SQUARE_SHIFT) |
                 (constants::PAWN.value() << PROMOTION_TYPE_SHIFT) |
                 (std::uint32_t(type) << MOVE_TYPE_SHIFT)) {}
    constexpr inline Move(Square from_square, Square to_square, PieceType promotion_pt,
                          Move::Type type = Move::Type::NONE)
        : value_(from_square.value() | (to_square.value() << TO_SQUARE_SHIFT) |
                 (promotion_pt.value() << PROMOTION_TYPE_SHIFT) |
                 (std::uint32_t(type) << MOVE_TYPE_SHIFT)) {}

    static inline std::optional<Move> from(const std::string& str) {
        auto strlen = str.length();
        if (strlen > 5 || strlen < 4) {
            return std::nullopt;
        }
        auto from = Square::from(str.substr(0, 2));
        auto to = Square::from(str.substr(2, 4));
        if (!(from && to)) {
            return std::nullopt;
        }

        if (str.size() == 5) {
            auto promotion_pt = PieceType::from(str[4]);
            if (!promotion_pt) {
                return std::nullopt;
            }
            return Move{*from, *to, *promotion_pt};
        }
        return Move{*from, *to};
    }

    inline std::string to_str() const {
        std::string move_str = from_square().to_str() + to_square().to_str();
        auto promotion_pt = promotion_piece_type();
        if (promotion_pt) {
            move_str += promotion_pt->to_char();
        }
        return move_str;
    }

    constexpr inline bool operator==(const Move rhs) const {
        return value_sans_type() == rhs.value_sans_type();
    }
    constexpr inline bool operator!=(const Move rhs) const {
        return value_sans_type() != rhs.value_sans_type();
    }

    constexpr inline Square from_square() const { return value() & 0x3f; }
    constexpr inline Square to_square() const { return (value() & 0xfc0) >> 6; }
    constexpr inline Move::Type type() const {
        return static_cast<Move::Type>((value() & MOVE_TYPE_MASK) >> MOVE_TYPE_SHIFT);
    }
    constexpr inline std::optional<PieceType> promotion_piece_type() const {
        PieceType promotion_pt = (value() & PROMOTION_TYPE_MASK) >> PROMOTION_TYPE_SHIFT;
        if (promotion_pt == constants::PAWN || promotion_pt == constants::KING) {
            return std::nullopt;
        }
        return promotion_pt;
    }

    constexpr inline value_type value_sans_type() const { return value_ & ~MOVE_TYPE_MASK; }
    constexpr inline value_type value() const { return value_; }

  private:
    value_type value_;
};

class MoveList {
  public:
    using value_type = std::vector<Move>;
    using iterator = value_type::iterator;
    using const_iterator = value_type::const_iterator;

    inline MoveList() { values_.reserve(32); }

    inline iterator begin() { return values_.begin(); }
    inline iterator end() { return values_.end(); }
    inline const_iterator cbegin() const { return values_.cbegin(); }
    inline const_iterator cend() const { return values_.cend(); }

    inline void decrement_size() { values_.pop_back(); }
    inline void add(Move move) { values_.push_back(move); }
    template <class F> inline void sort(F move_evaluator) {
        auto& moves = values_mut_ref();
        std::vector<int> scores;
        scores.reserve(values_.size());
        for (int i = 0; i < size(); ++i) {
            scores[i] = move_evaluator(moves[i]);
        }
        for (int i = 1; i < size(); ++i) {
            Move moving_move = moves[i];
            int moving_score = scores[i];
            int j = i;
            for (; j > 0; --j) {
                if (scores[j - 1] < moving_score) {
                    scores[j] = scores[j - 1];
                    moves[j] = moves[j - 1];
                } else {
                    break;
                }
            }
            scores[j] = moving_score;
            moves[j] = moving_move;
        }
    }
    inline int size() const { return values_.size(); }
    inline const value_type& values() const { return values_; }

  protected:
    inline value_type& values_mut_ref() { return values_; }

  private:
    value_type values_;
};

inline std::ostream& operator<<(std::ostream& ostream, Move move) {
    return ostream << move.to_str();
}

} // namespace libchess

namespace std {

template <> struct hash<libchess::Move> : public hash<libchess::Move::value_type> {};

} // namespace std

#endif // LIBCHESS_MOVE_H
