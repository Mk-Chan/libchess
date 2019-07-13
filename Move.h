#ifndef LIBCHESS_MOVE_H
#define LIBCHESS_MOVE_H

#include <algorithm>
#include <optional>
#include <vector>

#include "PieceType.h"
#include "Square.h"

namespace libchess {

/// Represents a chess move.
/** A move consists of the following parts.
 * 1. From Square
 * 2. To Square
 * 3. Promotion PieceType
 *
 * Important points to remember:
 * - A Move can be made on a Position using Position.make_move.
 * - Making an invalid Move on a Position is undefined behavior.
 * - Any move can be checked for validity using Position.is_legal_move.
 * - `Move`s from Position.pseudo_legal_move_list can be checked for validity
 *      much faster using Position.is_legal_generated_move.
 * - `Move`s from Position.legal_move_list are always valid for the given
 *      Position.
 **/
class Move {

  private:
    enum BitOpLookup : std::uint32_t {
        TO_SQUARE_SHIFT = 6,
        PROMOTION_TYPE_SHIFT = 12,
        MOVE_TYPE_SHIFT = 15,

        PROMOTION_TYPE_MASK = 7 << PROMOTION_TYPE_SHIFT,
        MOVE_TYPE_MASK = 7 << MOVE_TYPE_SHIFT
    };

  public:
    /// Set of possible Move types.
    enum class Type : std::uint8_t {
        NONE,
        NORMAL,
        CASTLING,
        ENPASSANT,
        PROMOTION,
        DOUBLE_PUSH,
        CAPTURE,
        CAPTURE_PROMOTION,
    };

    using value_type = int;

    constexpr explicit Move(std::uint32_t value) noexcept : value_(value) {}
    constexpr Move(Square from_square, Square to_square,
                   Move::Type type = Move::Type::NONE) noexcept
        : value_(from_square.value() | (to_square.value() << TO_SQUARE_SHIFT) |
                 (constants::PAWN.value() << PROMOTION_TYPE_SHIFT) |
                 (std::uint32_t(type) << MOVE_TYPE_SHIFT)) {}
    constexpr Move(Square from_square, Square to_square, PieceType promotion_pt,
                   Move::Type type = Move::Type::NONE) noexcept
        : value_(from_square.value() | (to_square.value() << TO_SQUARE_SHIFT) |
                 (promotion_pt.value() << PROMOTION_TYPE_SHIFT) |
                 (std::uint32_t(type) << MOVE_TYPE_SHIFT)) {}

    /// Parses Move from a string.
    [[nodiscard]] static std::optional<Move> from(const std::string& str) noexcept {
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

    /// The string representation of a Move in UCI format.
    [[nodiscard]] std::string to_str() const noexcept {
        std::string move_str = from_square().to_str() + to_square().to_str();
        auto promotion_pt = promotion_piece_type();
        if (promotion_pt) {
            move_str += promotion_pt->to_char();
        }
        return move_str;
    }

    [[nodiscard]] constexpr bool operator==(const Move rhs) const noexcept {
        return value_sans_type() == rhs.value_sans_type();
    }
    [[nodiscard]] constexpr bool operator!=(const Move rhs) const noexcept {
        return value_sans_type() != rhs.value_sans_type();
    }

    /// The from Square of the move.
    [[nodiscard]] constexpr Square from_square() const noexcept { return Square{value() & 0x3f}; }

    /// The to Square of the move.
    [[nodiscard]] constexpr Square to_square() const noexcept {
        return Square{(value() & 0xfc0) >> 6};
    }

    /// The promotion PieceType.
    /**
     * One of {PieceType::Value::QUEEN, PieceType::Value::KNIGHT, PieceType::Value::BISHOP,
     *  PieceType::Value::ROOK} or empty.
     **/
    [[nodiscard]] constexpr std::optional<PieceType> promotion_piece_type() const noexcept {
        PieceType promotion_pt =
            PieceType{int((value() & PROMOTION_TYPE_MASK) >> PROMOTION_TYPE_SHIFT)};
        if (promotion_pt == constants::PAWN || promotion_pt == constants::KING) {
            return std::nullopt;
        }
        return promotion_pt;
    }

    /// The type of the move.
    /// Will be Move::Type::UNKNOWN if the move was not generated or constructed with it.
    [[nodiscard]] constexpr Move::Type type() const noexcept {
        return static_cast<Move::Type>((value() & MOVE_TYPE_MASK) >> MOVE_TYPE_SHIFT);
    }

    /// Internal value of the Move ignoring Move::Type.
    [[nodiscard]] constexpr value_type value_sans_type() const noexcept {
        return value_ & ~MOVE_TYPE_MASK;
    }

    /// Internal value of the Move. Will include Move::Type if provided.
    [[nodiscard]] constexpr value_type value() const noexcept { return value_; }

  private:
    value_type value_;
};

/// Represents a vector-like container for `Move`s.
class MoveList {
  public:
    using value_type = std::vector<Move>;
    using iterator = value_type::iterator;
    using const_iterator = value_type::const_iterator;

    MoveList() noexcept { values_.reserve(32); }

    [[nodiscard]] iterator begin() noexcept { return values_.begin(); }
    [[nodiscard]] iterator end() noexcept { return values_.end(); }
    [[nodiscard]] const_iterator cbegin() const noexcept { return values_.cbegin(); }
    [[nodiscard]] const_iterator cend() const noexcept { return values_.cend(); }

    void pop_back() noexcept { values_.pop_back(); }
    void add(Move move) noexcept { values_.push_back(move); }
    template <class F> void sort(F move_evaluator) noexcept {
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
    [[nodiscard]] int size() const noexcept { return values_.size(); }
    [[nodiscard]] const value_type& values() const noexcept { return values_; }

    [[nodiscard]] bool contains(Move move) const noexcept {
        return std::find(cbegin(), cend(), move) != cend();
    }

  protected:
    [[nodiscard]] value_type& values_mut_ref() noexcept { return values_; }

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
