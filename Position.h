#ifndef LIBCHESS_POSITION_H
#define LIBCHESS_POSITION_H

#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

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

namespace constants {

static std::string STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

}  // namespace constants

class Position {
   private:
    Position() : side_to_move_(constants::WHITE), ply_(0) {
    }

   public:
    explicit Position(const std::string& fen_str) : Position() {
        *this = *Position::from_fen(fen_str);
    }
    using hash_type = std::uint64_t;

    enum class GameState
    {
        IN_PROGRESS,
        CHECKMATE,
        STALEMATE,
        THREEFOLD_REPETITION,
        FIFTY_MOVES
    };

    // Getters
    Bitboard piece_type_bb(PieceType piece_type) const;
    Bitboard piece_type_bb(PieceType piece_type, Color color) const;
    Bitboard color_bb(Color color) const;
    Bitboard occupancy_bb() const;
    Color side_to_move() const;
    CastlingRights castling_rights() const;
    std::optional<Square> enpassant_square() const;
    std::optional<Move> previous_move() const;
    std::optional<PieceType> previously_captured_piece() const;
    std::optional<PieceType> piece_type_on(Square square) const;
    std::optional<Color> color_of(Square square) const;
    std::optional<Piece> piece_on(Square square) const;
    hash_type hash() const;
    hash_type pawn_hash() const;
    Square king_square(Color color) const;
    int halfmoves() const;
    int fullmoves() const;
    bool in_check() const;
    bool is_repeat(int times = 1) const;
    int repeat_count() const;
    const std::string& start_fen() const;
    GameState game_state() const;

    // Move Integration
    Move::Type move_type_of(Move move) const;
    bool is_capture_move(Move move) const;
    bool is_promotion_move(Move move) const;
    bool is_legal_move(Move move) const;
    bool is_legal_generated_move(Move move) const;
    void unmake_move();
    void make_move(Move move);
    void make_null_move();

    // Attacks
    Bitboard checkers_to(Color c) const;
    Bitboard attackers_to(Square square) const;
    Bitboard attackers_to(Square square, Color c) const;
    Bitboard attackers_to(Square square, Bitboard occupancy) const;
    Bitboard attackers_to(Square square, Bitboard occupancy, Color c) const;
    Bitboard attacks_of_piece_on(Square square) const;
    Bitboard pinned_pieces_of(Color c) const;

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
    void display_raw(std::ostream& ostream = std::cout) const;
    void display(std::ostream& ostream = std::cout) const;
    std::string fen() const;
    std::string uci_line() const;
    void vflip();
    std::optional<Move> smallest_capture_move_to(Square square) const;
    int see_to(Square square, std::array<int, 6> piece_values);
    int see_for(Move move, std::array<int, 6> piece_values);
    static std::optional<Position> from_fen(const std::string& fen);
    static std::optional<Position> from_uci_position_line(const std::string& line);

   protected:
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
        CastlingRights castling_rights_;
        std::optional<Square> enpassant_square_;
        std::optional<Move> previous_move_;
        std::optional<PieceType> captured_pt_;
        Move::Type move_type_ = Move::Type::NONE;
        hash_type hash_ = 0;
        hash_type pawn_hash_ = 0;
        int halfmoves_ = 0;
    };

    int ply() const {
        return ply_;
    }
    const std::vector<State>& history() const {
        return history_;
    }
    State& state_mut_ref() {
        return history_[ply()];
    }
    State& state_mut_ref(int ply) {
        return history_[ply];
    }
    const State& state() const {
        return history_[ply()];
    }
    const State& state(int ply) const {
        return history_[ply];
    }
    hash_type calculate_hash() const {
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
        auto ep_sq = enpassant_square();
        if (ep_sq) {
            hash_value ^= zobrist::enpassant_key(*ep_sq);
        }
        hash_value ^= zobrist::castling_rights_key(castling_rights());
        hash_value ^= zobrist::side_to_move_key(side_to_move());
        return hash_value;
    }
    hash_type calculate_pawn_hash() const {
        hash_type hash_value = 0;
        for (Color c : constants::COLORS) {
            Bitboard bb = piece_type_bb(constants::PAWN, c);
            while (bb) {
                hash_value ^= zobrist::piece_square_key(bb.forward_bitscan(), constants::PAWN, c);
                bb.forward_popbit();
            }
        }
        return hash_value;
    }

    void put_piece(Square square, PieceType piece_type, Color color) {
        Bitboard square_bb = Bitboard{square};
        piece_type_bb_[piece_type.value()] |= square_bb;
        color_bb_[color.value()] |= square_bb;
    }
    void remove_piece(Square square, PieceType piece_type, Color color) {
        Bitboard square_bb = Bitboard{square};
        piece_type_bb_[piece_type.value()] &= ~square_bb;
        color_bb_[color.value()] &= ~square_bb;
    }
    void move_piece(Square from_square, Square to_square, PieceType piece_type, Color color) {
        Bitboard from_to_sqs_bb = Bitboard{from_square} ^ Bitboard { to_square };
        piece_type_bb_[piece_type.value()] ^= from_to_sqs_bb;
        color_bb_[color.value()] ^= from_to_sqs_bb;
    }
    void reverse_side_to_move() {
        side_to_move_ = !side_to_move_;
    }

   private:
    Bitboard piece_type_bb_[6];
    Bitboard color_bb_[2];
    Color side_to_move_;
    int fullmoves_;
    int ply_;
    std::vector<State> history_;

    std::string start_fen_;
};

}  // namespace libchess

#include "Position/Attacks.h"
#include "Position/Getters.h"
#include "Position/MoveGeneration.h"
#include "Position/MoveIntegration.h"
#include "Position/Utilities.h"

#endif  // LIBCHESS_POSITION_H
