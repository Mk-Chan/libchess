#ifndef LIBCHESS_POSITION_H
#define LIBCHESS_POSITION_H

#include <array>
#include <cctype>
#include <sstream>
#include <string>
#include <tuple>

#include "Bitboard.h"
#include "CastlingRights.h"
#include "Color.h"
#include "Lookups.h"
#include "Move.h"
#include "Piece.h"
#include "PieceType.h"
#include "Square.h"
#include "internal/Zobrist.h"

namespace libchess {

class Position {
  public:
    using hash_type = std::uint64_t;

    // Constructors
    Position();
    Position(const std::string& fen);

    // Getters
    Bitboard piece_type_bb(PieceType piece_type) const;
    Bitboard piece_type_bb(PieceType piece_type, Color color) const;
    Bitboard color_bb(Color color) const;
    Bitboard occupancy_bb() const;
    Color side_to_move() const;
    CastlingRights castling_rights() const;
    Square enpassant_square() const;
    int halfmoves() const;
    int fullmoves() const;
    Move previous_move() const;
    PieceType previously_captured_piece() const;
    hash_type hash() const;
    Square king_square(Color color) const;
    PieceType piece_type_on(Square square) const;
    Color color_of(Square square) const;
    Piece piece_on(Square square) const;
    bool in_check() const;
    bool is_repeat(int times = 1) const;
    int repeat_count() const;
    std::string fen() const;

    // Move Integration
    Move::Type move_type_of(Move move) const;
    bool is_capture_move(Move move) const;
    bool is_promotion_move(Move move) const;
    void unmake_move();
    void make_move(Move move);
    void make_null_move();

    // Attacks
    Bitboard checkers_to(Color to_color) const;
    Bitboard attackers_to(Square square) const;
    Bitboard attackers_to(Square square, Color attacker_color) const;
    Bitboard attackers_to(Square square, Color attacker_color, Bitboard occupancy) const;
    Bitboard pinned_pieces(Color to_color) const;

    // Move Generation
    void generate_quiet_promotions(MoveList& move_list, Color stm) const;
    void generate_capture_promotions(MoveList& move_list, Color stm) const;
    void generate_promotions(MoveList& move_list, Color stm) const;
    void generate_pawn_quiets(MoveList& move_list, Color stm) const;
    void generate_pawn_captures(MoveList& move_list, Color stm) const;
    void generate_pawn_moves(MoveList& move_list, Color stm) const;
    void generate_non_pawn_quiets(PieceType pt, MoveList& move_list, Color stm) const;
    void generate_non_pawn_captures(PieceType pt, MoveList& move_list, Color stm) const;
    void generate_knight_moves(MoveList& move_list, Color stm) const;
    void generate_bishop_moves(MoveList& move_list, Color stm) const;
    void generate_rook_moves(MoveList& move_list, Color stm) const;
    void generate_queen_moves(MoveList& move_list, Color stm) const;
    void generate_king_moves(MoveList& move_list, Color stm) const;
    void generate_castling(MoveList& move_list, Color stm) const;
    void generate_checker_block_moves(MoveList& move_list, Color stm) const;
    void generate_checker_capture_moves(MoveList& move_list, Color stm) const;
    void generate_quiet_moves(MoveList& move_list, Color stm) const;
    void generate_capture_moves(MoveList& move_list, Color stm) const;
    MoveList check_evasion_move_list(Color stm) const;
    MoveList pseudo_legal_move_list(Color stm) const;
    MoveList legal_move_list(Color stm) const;
    MoveList check_evasion_move_list() const;
    MoveList pseudo_legal_move_list() const;
    MoveList legal_move_list() const;

    // Utilities
    inline void display_raw(std::ostream& ostream = std::cout) const;
    inline void display(std::ostream& ostream = std::cout) const;

  protected:
    constexpr static int history_max_size = 512;
    // clang-format off
    constexpr static int castling_spoilers[64] = {
        13, 15, 15, 15, 12, 15, 15, 14,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        7,  15, 15, 15, 3,  15, 15, 11
    };
    // clang-format on

    struct State {
        CastlingRights castling_rights_ = constants::CASTLING_RIGHT_NONE;
        Square enpassant_square_ = constants::SQUARE_NONE;
        Move previous_move_ = constants::MOVE_NONE;
        PieceType captured_pt_ = constants::PIECE_TYPE_NONE;
        Move::Type move_type_ = Move::Type::NONE;
        hash_type hash_ = 0;
        int halfmoves_ = 0;
    };

    inline int ply() const { return ply_; }
    inline const std::array<State, history_max_size>& history() const { return history_; }
    inline State& state_mut_ref() { return history_[ply()]; }
    inline State& state_mut_ref(int ply) { return history_[ply]; }
    inline const State& state() const { return history_[ply()]; }
    inline const State& state(int ply) const { return history_[ply]; }
    inline hash_type calculate_hash() const {
        hash_type hash_value = 0;
        for (Color c : constants::COLORS) {
            for (PieceType pt : constants::PIECE_TYPES) {
                Bitboard bb = piece_type_bb(pt, c);
                while (bb) {
                    hash_value ^= zobrist::piece_square_key(bb.forward_bitscan(), pt, c);
                    bb.forward_popbit();
                }
            }
        }
        if (enpassant_square() != constants::SQUARE_NONE) {
            hash_value ^= zobrist::enpassant_key(enpassant_square());
        }
        hash_value ^= zobrist::castling_rights_key(castling_rights());
        hash_value ^= zobrist::side_to_move_key(side_to_move());
        return hash_value;
    }

    inline void put_piece(Square square, PieceType piece_type, Color color) {
        Bitboard square_bb = Bitboard{square};
        piece_type_bb_[piece_type.value()] |= square_bb;
        color_bb_[color.value()] |= square_bb;
    }
    inline void remove_piece(Square square, PieceType piece_type, Color color) {
        Bitboard square_bb = Bitboard{square};
        piece_type_bb_[piece_type.value()] &= ~square_bb;
        color_bb_[color.value()] &= ~square_bb;
    }
    inline void move_piece(Square from_square, Square to_square, PieceType piece_type,
                           Color color) {
        Bitboard from_to_sqs_bb = Bitboard{from_square} ^ Bitboard { to_square };
        piece_type_bb_[piece_type.value()] ^= from_to_sqs_bb;
        color_bb_[color.value()] ^= from_to_sqs_bb;
    }
    inline void reverse_side_to_move() { side_to_move_ = !side_to_move_; }

    inline bool is_legal_move(Move move) const {
        Color c = side_to_move();
        Square from = move.from_square();
        Square king_sq = king_square(c);
        if (move.type() == Move::Type::ENPASSANT) {
            Bitboard ep_bb = Bitboard{enpassant_square()};
            Bitboard post_ep_occupancy =
                (occupancy_bb() ^ Bitboard{from} ^ lookups::pawn_shift(ep_bb, !c)) | ep_bb;

            return !(lookups::rook_attacks(king_sq, post_ep_occupancy) & color_bb(!c) &
                     (piece_type_bb(constants::QUEEN) | piece_type_bb(constants::ROOK))) &&
                   !(lookups::bishop_attacks(king_sq, post_ep_occupancy) & color_bb(!c) &
                     ((piece_type_bb(constants::QUEEN) | piece_type_bb(constants::BISHOP))));
        } else if (from == king_sq) {
            return move.type() == Move::Type::CASTLING || !attackers_to(move.to_square(), !c);
        } else {
            return !(pinned_pieces(c) & Bitboard{from}) ||
                   (Bitboard{move.to_square()} & lookups::direction_xray(king_sq, from));
        }
    }

  private:
    Bitboard piece_type_bb_[constants::NUM_PIECE_TYPES];
    Bitboard color_bb_[constants::NUM_COLORS];
    Color side_to_move_;
    int fullmoves_;
    int ply_;
    std::array<State, history_max_size> history_;
};

namespace constants {

static std::string STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

}

} // namespace libchess

#include "Position/Attacks.h"
#include "Position/Constructors.h"
#include "Position/Getters.h"
#include "Position/MoveGeneration.h"
#include "Position/MoveIntegration.h"
#include "Position/Utilities.h"

#endif // LIBCHESS_POSITION_H
