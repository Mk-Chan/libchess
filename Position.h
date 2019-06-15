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

    inline Position(const std::string& fen) : ply_(0) {
        State& curr_state = state_mut_ref();

        std::stringstream fen_stream{fen};
        std::string fen_part;

        // Piece list
        fen_stream >> fen_part;
        Square current_square = constants::A8;
        for (char c : fen_part) {
            if (c >= '1' && c <= '9') {
                current_square += (c - '0');
            } else if (c == '/') {
                current_square -= 16;
            } else {
                auto piece = Piece::from(c);
                put_piece(current_square, piece.type(), piece.color());
                ++current_square;
            }
        }

        // Side to move
        fen_stream >> fen_part;
        side_to_move_ = Color::from(fen_part[0]);

        // Castling rights
        fen_stream >> fen_part;
        curr_state.castling_rights_ = CastlingRights::from(fen_part);

        // Enpassant square
        fen_stream >> fen_part;
        curr_state.enpassant_square_ = Square::from(fen_part);

        // Halfmoves
        fen_stream >> fen_part;
        const char* fen_part_cstr = fen_part.c_str();
        char* end;
        curr_state.halfmoves_ = std::strtol(fen_part_cstr, &end, 10);

        // Fullmoves
        fen_stream >> fen_part;
        fullmoves_ = std::strtol(fen_part_cstr, &end, 10);

        curr_state.hash_ = calculate_hash();
    }
    inline Position() : fullmoves_(1), ply_(0) {}

    inline Bitboard piece_type_bb(PieceType piece_type) const {
        return piece_type_bb_[piece_type.value()];
    }
    inline Bitboard piece_type_bb(PieceType piece_type, Color color) const {
        return piece_type_bb_[piece_type.value()] & color_bb(color);
    }
    inline Bitboard color_bb(Color color) const { return color_bb_[color.value()]; }
    inline Bitboard occupancy_bb() const {
        return color_bb(constants::WHITE) | color_bb(constants::BLACK);
    }
    inline Color side_to_move() const { return side_to_move_; }
    inline CastlingRights castling_rights() const { return history_[ply_].castling_rights_; }
    inline Square enpassant_square() const { return history_[ply_].enpassant_square_; }
    inline int halfmoves() const { return history_[ply_].halfmoves_; }
    inline int fullmoves() const { return fullmoves_; }
    inline Move previous_move() const { return history_[ply_].previous_move_; }
    inline PieceType previously_captured_piece() const { return history_[ply_].captured_pt_; }
    inline hash_type hash() const { return history_[ply_].hash_; }

    inline Square king_square(Color color) const {
        return piece_type_bb(constants::KING, color).forward_bitscan();
    }
    inline PieceType piece_type_on(Square square) const {
        for (PieceType piece_type : constants::PIECE_TYPES) {
            if (piece_type_bb(piece_type) & Bitboard{square}) {
                return piece_type;
            }
        }
        return constants::PIECE_TYPE_NONE;
    }
    inline Color color_of(Square square) const {
        for (Color color : constants::COLORS) {
            if (color_bb(color) & Bitboard{square}) {
                return color;
            }
        }
        return constants::COLOR_NONE;
    }
    inline Piece piece_on(Square square) const {
        return Piece{piece_type_on(square), color_of(square)};
    }

    inline Bitboard pinned_pieces(Color to_color) const {
        Bitboard pinned_bb;
        Square king_sq = king_square(to_color);
        Bitboard pinners_bb =
            ((piece_type_bb(constants::QUEEN) | piece_type_bb(constants::ROOK)) &
             color_bb(!to_color) & lookups::rook_attacks(king_sq)) |
            ((piece_type_bb(constants::QUEEN) | piece_type_bb(constants::BISHOP)) &
             color_bb(!to_color) & lookups::bishop_attacks(king_sq));
        while (pinners_bb) {
            Square sq = pinners_bb.forward_bitscan();
            pinners_bb.forward_popbit();
            Bitboard bb = lookups::intervening(sq, king_sq) & occupancy_bb();
            if (bb.popcount() == 1) {
                pinned_bb ^= bb & color_bb(to_color);
            }
        }
        return pinned_bb;
    }
    inline Bitboard checkers_to(Color to_color) const {
        return attackers_to(piece_type_bb(constants::KING, to_color).forward_bitscan(), !to_color);
    }
    inline bool in_check() const { return checkers_to(side_to_move()) != 0; }
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
    inline Move::Type move_type_of(Move move) const {
        Move::Type move_type = move.type();
        if (move_type != constants::MOVE_TYPE_NONE) {
            return move_type;
        } else {
            Square to_square = move.to_square();
            Square from_square = move.from_square();
            PieceType moving_pt = piece_type_on(from_square);
            PieceType captured_pt = piece_type_on(to_square);
            PieceType promotion_pt = move.promotion_piece_type();
            if (promotion_pt != constants::PIECE_TYPE_NONE) {
                return captured_pt != constants::PIECE_TYPE_NONE ? Move::Type::CAPTURE_PROMOTION
                                                                 : Move::Type::PROMOTION;
            } else if (captured_pt != constants::PIECE_TYPE_NONE) {
                return Move::Type::CAPTURE;
            } else if (moving_pt == constants::PAWN) {
                int sq_diff = std::abs(to_square - from_square);
                if (sq_diff == 16) {
                    return Move::Type::DOUBLE_PUSH;
                } else if (sq_diff == 9 || sq_diff == 7) {
                    return Move::Type::ENPASSANT;
                } else {
                    return Move::Type::NORMAL;
                }
            } else if (moving_pt == constants::KING && std::abs(to_square - from_square) == 2) {
                return Move::Type::CASTLING;
            } else {
                return Move::Type::NORMAL;
            }
        }
    }
    inline bool is_capture_move(Move move) const {
        Move::Type move_type = move.type();
        switch (move_type) {
        case Move::Type::CAPTURE:
        case Move::Type::CAPTURE_PROMOTION:
        case Move::Type::ENPASSANT:
            return true;
        default:
            return false;
        }
    }
    inline bool is_promotion_move(Move move) const {
        Move::Type move_type = move.type();
        switch (move_type) {
        case Move::Type::PROMOTION:
        case Move::Type::CAPTURE_PROMOTION:
            return true;
        default:
            return false;
        }
    }
    inline void unmake_move() {
        if (side_to_move() == constants::WHITE) {
            --fullmoves_;
        }
        Move move = state().previous_move_;
        Move::Type move_type = state().move_type_;
        PieceType captured_pt = state().captured_pt_;
        --ply_;
        reverse_side_to_move();
        if (move == constants::MOVE_NONE) {
            return;
        }
        Color stm = side_to_move();

        Square from_square = move.from_square();
        Square to_square = move.to_square();

        PieceType moving_pt = piece_type_on(to_square);
        switch (move_type) {
        case Move::Type::NORMAL:
            move_piece(to_square, from_square, moving_pt, stm);
            break;
        case Move::Type::CAPTURE:
            move_piece(to_square, from_square, moving_pt, stm);
            put_piece(to_square, captured_pt, !stm);
            break;
        case Move::Type::DOUBLE_PUSH:
            move_piece(to_square, from_square, constants::PAWN, stm);
            break;
        case Move::Type::ENPASSANT:
            put_piece(stm == constants::WHITE ? Square{to_square - 8} : Square(to_square + 8),
                      constants::PAWN, !stm);
            move_piece(to_square, from_square, constants::PAWN, stm);
            break;
        case Move::Type::CASTLING:
            move_piece(to_square, from_square, constants::KING, stm);
            switch (to_square) {
            case constants::C1:
                move_piece(constants::D1, constants::A1, constants::ROOK, stm);
                break;
            case constants::G1:
                move_piece(constants::F1, constants::H1, constants::ROOK, stm);
                break;
            case constants::C8:
                move_piece(constants::D8, constants::A8, constants::ROOK, stm);
                break;
            case constants::G8:
                move_piece(constants::F8, constants::H8, constants::ROOK, stm);
                break;
            default:
                break;
            }
            break;
        case Move::Type::PROMOTION:
            remove_piece(to_square, move.promotion_piece_type(), stm);
            put_piece(from_square, constants::PAWN, stm);
            break;
        case Move::Type::CAPTURE_PROMOTION:
            remove_piece(to_square, move.promotion_piece_type(), stm);
            put_piece(from_square, constants::PAWN, stm);
            put_piece(to_square, captured_pt, !stm);
            break;
        case Move::Type::NONE:
            break;
        }
    }
    inline void make_move(Move move) {
        Color stm = side_to_move();
        if (stm == constants::BLACK) {
            ++fullmoves_;
        }
        State& prev_state = state_mut_ref();
        ++ply_;
        State& next_state = state_mut_ref();
        next_state.halfmoves_ = prev_state.halfmoves_ + 1;
        next_state.previous_move_ = move;
        next_state.enpassant_square_ = constants::SQUARE_NONE;

        Square from_square = move.from_square();
        Square to_square = move.to_square();

        next_state.castling_rights_ = CastlingRights{prev_state.castling_rights_.value() &
                                                     castling_spoilers[from_square.value()] &
                                                     castling_spoilers[to_square.value()]};

        PieceType moving_pt = piece_type_on(from_square);
        PieceType captured_pt = piece_type_on(to_square);
        PieceType promotion_pt = move.promotion_piece_type();

        Move::Type move_type = move_type_of(move);

        if (moving_pt == constants::PAWN || captured_pt != constants::PIECE_TYPE_NONE) {
            next_state.halfmoves_ = 0;
        }

        switch (move_type) {
        case Move::Type::NORMAL:
            move_piece(from_square, to_square, moving_pt, stm);
            break;
        case Move::Type::CAPTURE:
            remove_piece(to_square, captured_pt, !stm);
            move_piece(from_square, to_square, moving_pt, stm);
            break;
        case Move::Type::DOUBLE_PUSH:
            move_piece(from_square, to_square, constants::PAWN, stm);
            next_state.enpassant_square_ =
                stm == constants::WHITE ? Square(from_square + 8) : Square(from_square - 8);
            break;
        case Move::Type::ENPASSANT:
            move_piece(from_square, to_square, constants::PAWN, stm);
            remove_piece(stm == constants::WHITE ? Square(to_square - 8) : Square(to_square + 8),
                         constants::PAWN, !stm);
            break;
        case Move::Type::CASTLING:
            move_piece(from_square, to_square, constants::KING, stm);
            switch (to_square) {
            case constants::C1:
                move_piece(constants::A1, constants::D1, constants::ROOK, stm);
                break;
            case constants::G1:
                move_piece(constants::H1, constants::F1, constants::ROOK, stm);
                break;
            case constants::C8:
                move_piece(constants::A8, constants::D8, constants::ROOK, stm);
                break;
            case constants::G8:
                move_piece(constants::H8, constants::F8, constants::ROOK, stm);
                break;
            default:
                break;
            }
            break;
        case Move::Type::PROMOTION:
            remove_piece(from_square, constants::PAWN, stm);
            put_piece(to_square, promotion_pt, stm);
            break;
        case Move::Type::CAPTURE_PROMOTION:
            remove_piece(to_square, captured_pt, !stm);
            remove_piece(from_square, constants::PAWN, stm);
            put_piece(to_square, promotion_pt, stm);
            break;
        case Move::Type::NONE:
            break;
        }
        next_state.captured_pt_ = captured_pt;
        next_state.move_type_ = move_type;
        reverse_side_to_move();
        next_state.hash_ = calculate_hash();
    }
    inline void make_null_move() {
        if (side_to_move() == constants::BLACK) {
            ++fullmoves_;
        }
        State& prev = state_mut_ref();
        ++ply_;
        State& next = state_mut_ref();
        reverse_side_to_move();
        next.previous_move_ = constants::MOVE_NONE;
        next.halfmoves_ = prev.halfmoves_ + 1;
        next.enpassant_square_ = constants::SQUARE_NONE;
        next.castling_rights_ = prev.castling_rights_;
        next.hash_ = calculate_hash();
    }

    inline Bitboard attackers_to(Square square) const {
        Bitboard attackers;
        attackers |= (lookups::pawn_attacks(square, constants::WHITE) |
                      lookups::pawn_attacks(square, constants::BLACK)) &
                     piece_type_bb(constants::PAWN);
        Bitboard occupancy = occupancy_bb();
        for (PieceType pt = constants::KNIGHT; pt <= constants::KING; ++pt) {
            attackers |=
                lookups::non_pawn_piece_type_attacks(pt, square, occupancy) & piece_type_bb(pt);
        }
        return attackers;
    }
    inline Bitboard attackers_to(Square square, Color attacker_color) const {
        Bitboard attackers;
        attackers |= lookups::pawn_attacks(square, !attacker_color) &
                     piece_type_bb(constants::PAWN, attacker_color);
        Bitboard occupancy = occupancy_bb();
        for (PieceType pt = constants::KNIGHT; pt <= constants::KING; ++pt) {
            attackers |= lookups::non_pawn_piece_type_attacks(pt, square, occupancy) &
                         piece_type_bb(pt, attacker_color);
        }
        return attackers;
    }
    inline Bitboard attackers_to(Square square, Color attacker_color, Bitboard occupancy) const {
        Bitboard attackers;
        attackers |= lookups::pawn_attacks(square, !attacker_color) &
                     piece_type_bb(constants::PAWN, attacker_color);
        for (PieceType pt = constants::KNIGHT; pt <= constants::KING; ++pt) {
            attackers |= lookups::non_pawn_piece_type_attacks(pt, square, occupancy) &
                         piece_type_bb(pt, attacker_color);
        }
        return attackers;
    }

    inline void generate_quiet_promotions(MoveList& move_list) const {
        Color stm = side_to_move();
        Bitboard promotion_candidates =
            lookups::pawn_shift(piece_type_bb(constants::PAWN, stm) &
                                    lookups::relative_rank_mask(constants::RANK_7, stm),
                                stm) &
            ~occupancy_bb();
        while (promotion_candidates) {
            Square to_sq = promotion_candidates.forward_bitscan();
            promotion_candidates.forward_popbit();
            move_list.add(Move{lookups::pawn_shift(to_sq, !stm), to_sq, constants::QUEEN,
                               Move::Type::PROMOTION});
            move_list.add(Move{lookups::pawn_shift(to_sq, !stm), to_sq, constants::KNIGHT,
                               Move::Type::PROMOTION});
            move_list.add(Move{lookups::pawn_shift(to_sq, !stm), to_sq, constants::ROOK,
                               Move::Type::PROMOTION});
            move_list.add(Move{lookups::pawn_shift(to_sq, !stm), to_sq, constants::BISHOP,
                               Move::Type::PROMOTION});
        }
    }
    inline void generate_capture_promotions(MoveList& move_list) const {
        Bitboard pawn_bb = piece_type_bb(constants::PAWN);
        Color stm = side_to_move();
        pawn_bb &= color_bb(stm) & lookups::relative_rank_mask(constants::RANK_7, stm);
        Bitboard opp_occupancy = color_bb(!stm);
        while (pawn_bb) {
            Square from_sq = pawn_bb.forward_bitscan();
            pawn_bb.forward_popbit();
            Bitboard attacks_bb = lookups::pawn_attacks(from_sq, stm) & opp_occupancy;
            while (attacks_bb) {
                Square to_sq = attacks_bb.forward_bitscan();
                attacks_bb.forward_popbit();
                move_list.add(
                    Move{from_sq, to_sq, constants::QUEEN, Move::Type::CAPTURE_PROMOTION});
                move_list.add(
                    Move{from_sq, to_sq, constants::KNIGHT, Move::Type::CAPTURE_PROMOTION});
                move_list.add(Move{from_sq, to_sq, constants::ROOK, Move::Type::CAPTURE_PROMOTION});
                move_list.add(
                    Move{from_sq, to_sq, constants::BISHOP, Move::Type::CAPTURE_PROMOTION});
            }
        }
    }
    inline void generate_promotions(MoveList& move_list) const {
        generate_capture_promotions(move_list);
        generate_quiet_promotions(move_list);
    }
    inline void generate_pawn_quiets(MoveList& move_list) const {
        generate_quiet_promotions(move_list);
        Color stm = side_to_move();
        Bitboard occupancy = occupancy_bb();
        Bitboard single_push_pawn_bb =
            lookups::pawn_shift(piece_type_bb(constants::PAWN, stm) &
                                    ~lookups::relative_rank_mask(constants::RANK_7, stm),
                                stm) &
            ~occupancy;
        Bitboard double_push_pawn_bb =
            lookups::pawn_shift(
                single_push_pawn_bb & lookups::relative_rank_mask(constants::RANK_3, stm), stm) &
            ~occupancy;
        while (double_push_pawn_bb) {
            Square to_sq = double_push_pawn_bb.forward_bitscan();
            double_push_pawn_bb.forward_popbit();
            move_list.add(Move{lookups::pawn_shift(to_sq, !stm, 2), to_sq,
                               constants::PIECE_TYPE_NONE, Move::Type::DOUBLE_PUSH});
        }
        while (single_push_pawn_bb) {
            Square to_sq = single_push_pawn_bb.forward_bitscan();
            single_push_pawn_bb.forward_popbit();
            move_list.add(Move{lookups::pawn_shift(to_sq, !stm), to_sq, constants::PIECE_TYPE_NONE,
                               Move::Type::NORMAL});
        }
    }
    inline void generate_pawn_captures(MoveList& move_list) const {
        generate_capture_promotions(move_list);
        Bitboard pawn_bb = piece_type_bb(constants::PAWN);
        Square ep_sq = enpassant_square();
        if (ep_sq != constants::SQUARE_NONE) {
            Bitboard ep_candidates =
                pawn_bb & color_bb(side_to_move()) & lookups::pawn_attacks(ep_sq, !side_to_move());
            while (ep_candidates) {
                Square sq = ep_candidates.forward_bitscan();
                ep_candidates.forward_popbit();
                move_list.add(Move{sq, ep_sq, constants::PIECE_TYPE_NONE, Move::Type::ENPASSANT});
            }
        }
        Color stm = side_to_move();
        pawn_bb &= color_bb(stm) & ~lookups::relative_rank_mask(constants::RANK_7, stm);
        while (pawn_bb) {
            Square from_sq = pawn_bb.forward_bitscan();
            pawn_bb.forward_popbit();
            Bitboard attacks_bb = lookups::pawn_attacks(from_sq, stm) & color_bb(!stm);
            while (attacks_bb) {
                Square to_sq = attacks_bb.forward_bitscan();
                attacks_bb.forward_popbit();
                move_list.add(
                    Move{from_sq, to_sq, constants::PIECE_TYPE_NONE, Move::Type::CAPTURE});
            }
        }
    }
    inline void generate_pawn_moves(MoveList& move_list) const {
        generate_pawn_captures(move_list);
        generate_pawn_quiets(move_list);
    }
    inline void generate_non_pawn_quiets(PieceType pt, MoveList& move_list) const {
        Bitboard piece_bb = piece_type_bb(pt, side_to_move());
        Bitboard occupancy = occupancy_bb();
        while (piece_bb) {
            Square sq = piece_bb.forward_bitscan();
            piece_bb.forward_popbit();
            Bitboard atks = lookups::non_pawn_piece_type_attacks(pt, sq, occupancy) & ~occupancy;
            while (atks) {
                Square to_sq = atks.forward_bitscan();
                atks.forward_popbit();
                move_list.add(Move{sq, to_sq, constants::PIECE_TYPE_NONE, Move::Type::NORMAL});
            }
        }
    }
    inline void generate_non_pawn_captures(PieceType pt, MoveList& move_list) const {
        Bitboard piece_bb = piece_type_bb(pt, side_to_move());
        Bitboard occupancy = occupancy_bb();
        Bitboard opp_occupancy = color_bb(!side_to_move());
        while (piece_bb) {
            Square sq = piece_bb.forward_bitscan();
            piece_bb.forward_popbit();
            Bitboard atks = lookups::non_pawn_piece_type_attacks(pt, sq, occupancy) & opp_occupancy;
            while (atks) {
                Square to_sq = atks.forward_bitscan();
                atks.forward_popbit();
                move_list.add(Move{sq, to_sq, constants::PIECE_TYPE_NONE, Move::Type::CAPTURE});
            }
        }
    }
    inline void generate_knight_moves(MoveList& move_list) const {
        generate_non_pawn_quiets(constants::KNIGHT, move_list);
        generate_non_pawn_captures(constants::KNIGHT, move_list);
    }
    inline void generate_bishop_moves(MoveList& move_list) const {
        generate_non_pawn_quiets(constants::BISHOP, move_list);
        generate_non_pawn_captures(constants::BISHOP, move_list);
    }
    inline void generate_rook_moves(MoveList& move_list) const {
        generate_non_pawn_quiets(constants::ROOK, move_list);
        generate_non_pawn_captures(constants::ROOK, move_list);
    }
    inline void generate_queen_moves(MoveList& move_list) const {
        generate_non_pawn_quiets(constants::QUEEN, move_list);
        generate_non_pawn_captures(constants::QUEEN, move_list);
    }
    inline void generate_king_moves(MoveList& move_list) const {
        generate_non_pawn_quiets(constants::KING, move_list);
        generate_non_pawn_captures(constants::KING, move_list);
    }
    inline void generate_castling(MoveList& move_list) const {
        const int castling_possibilities[2][2] = {
            {constants::WHITE_KINGSIDE.value(), constants::WHITE_QUEENSIDE.value()},
            {constants::BLACK_KINGSIDE.value(), constants::BLACK_QUEENSIDE.value()},
        };
        const int castling_intermediate_sqs[2][2][2] = {
            {{constants::F1, constants::G1}, {constants::D1, constants::C1}},
            {{constants::F8, constants::G8}, {constants::D8, constants::C8}}};
        const int castling_king_sqs[2][2][2] = {
            {{constants::E1, constants::G1}, {constants::E1, constants::C1}},
            {{constants::E8, constants::G8}, {constants::E8, constants::C8}}};
        const Bitboard castle_mask[2][2] = {
            {(Bitboard{constants::F1} | Bitboard{constants::G1}),
             (Bitboard{constants::D1} | Bitboard{constants::C1} | Bitboard{constants::B1})},
            {(Bitboard{constants::F8} | Bitboard{constants::G8}),
             (Bitboard{constants::D8} | Bitboard{constants::C8} | Bitboard{constants::B8})}};

        Bitboard occupancy = occupancy_bb();
        int c = side_to_move().value();
        if ((castling_possibilities[c][0] & castling_rights().value()) &&
            !(castle_mask[c][0] & occupancy) && !(attackers_to(castling_king_sqs[c][0][0], !c)) &&
            !(attackers_to(castling_intermediate_sqs[c][0][0], !c)) &&
            !(attackers_to(castling_intermediate_sqs[c][0][1], !c))) {
            move_list.add(Move{castling_king_sqs[c][0][0], castling_king_sqs[c][0][1],
                               constants::PIECE_TYPE_NONE, Move::Type::CASTLING});
        }
        if ((castling_possibilities[c][1] & castling_rights().value()) &&
            !(castle_mask[c][1] & occupancy) && !(attackers_to(castling_king_sqs[c][0][0], !c)) &&
            !(attackers_to(castling_intermediate_sqs[c][1][0], !c)) &&
            !(attackers_to(castling_intermediate_sqs[c][1][1], !c))) {
            move_list.add(Move{castling_king_sqs[c][1][0], castling_king_sqs[c][1][1],
                               constants::PIECE_TYPE_NONE, Move::Type::CASTLING});
        }
    }
    inline void generate_checker_block_moves(MoveList& move_list) const {
        Color c = side_to_move();
        Bitboard checkers = checkers_to(c);
        if (checkers.popcount() > 1) {
            return;
        }

        Square king_sq = king_square(c);
        Square checker_pos = checkers.forward_bitscan();
        Bitboard checker_intercept_bb = lookups::intervening(king_sq, checker_pos);
        if (!checker_intercept_bb) {
            return;
        }

        Bitboard pawns = piece_type_bb(constants::PAWN, c);
        Bitboard shifted_intercepts = lookups::pawn_shift(checker_intercept_bb, !c);
        Bitboard single_push_pawn_blocks = shifted_intercepts & pawns;
        Bitboard double_push_pawn_blocks =
            lookups::pawn_shift(shifted_intercepts & ~occupancy_bb(), !c) & pawns &
            lookups::relative_rank_mask(constants::RANK_2, c);
        while (double_push_pawn_blocks) {
            Square pawn_sq = double_push_pawn_blocks.forward_bitscan();
            double_push_pawn_blocks.forward_popbit();
            move_list.add(Move{pawn_sq, lookups::pawn_shift(pawn_sq, c, 2),
                               constants::PIECE_TYPE_NONE, Move::Type::DOUBLE_PUSH});
        }
        while (single_push_pawn_blocks) {
            Square pawn_sq = single_push_pawn_blocks.forward_bitscan();
            single_push_pawn_blocks.forward_popbit();
            Square target_sq = lookups::pawn_shift(pawn_sq, c);
            if ((c == constants::WHITE && pawn_sq.rank() == constants::RANK_7) ||
                (c == constants::BLACK && pawn_sq.rank() == constants::RANK_2)) {
                move_list.add(Move{pawn_sq, target_sq, constants::QUEEN, Move::Type::PROMOTION});
                move_list.add(Move{pawn_sq, target_sq, constants::KNIGHT, Move::Type::PROMOTION});
                move_list.add(Move{pawn_sq, target_sq, constants::BISHOP, Move::Type::PROMOTION});
                move_list.add(Move{pawn_sq, target_sq, constants::ROOK, Move::Type::PROMOTION});
            } else {
                move_list.add(
                    Move{pawn_sq, target_sq, constants::PIECE_TYPE_NONE, Move::Type::NORMAL});
            }
        }

        Bitboard excluded_pieces_mask = ~(Bitboard{king_sq} | pawns);
        while (checker_intercept_bb) {
            Square sq = checker_intercept_bb.forward_bitscan();
            checker_intercept_bb.forward_popbit();
            Bitboard blockers = attackers_to(sq, c) & excluded_pieces_mask;
            while (blockers) {
                Square atker_sq = blockers.forward_bitscan();
                blockers.forward_popbit();
                move_list.add(Move{atker_sq, sq, constants::PIECE_TYPE_NONE, Move::Type::NORMAL});
            }
        }
    }
    inline void generate_checker_capture_moves(MoveList& move_list) const {
        Color c = side_to_move();
        Bitboard checkers = checkers_to(c);
        if (checkers.popcount() > 1) {
            return;
        }

        Bitboard pawns = piece_type_bb(constants::PAWN, c);
        Square ep_square = enpassant_square();
        if (ep_square != constants::SQUARE_NONE &&
            (lookups::pawn_shift(Bitboard{ep_square}, !c) & checkers)) {
            Bitboard ep_candidates = pawns & lookups::pawn_attacks(ep_square, !c);
            while (ep_candidates) {
                Square sq = ep_candidates.forward_bitscan();
                ep_candidates.forward_popbit();
                move_list.add(
                    Move{sq, ep_square, constants::PIECE_TYPE_NONE, Move::Type::ENPASSANT});
            }
        }

        Square checker_sq = checkers.forward_bitscan();
        Bitboard attackers = attackers_to(checker_sq, c) & ~Bitboard{king_square(c)};
        Bitboard rank7_pawns = pawns & lookups::relative_rank_mask(constants::RANK_7, c);
        Bitboard pawn_prom_attackers = attackers & rank7_pawns;
        while (pawn_prom_attackers) {
            Square sq = pawn_prom_attackers.forward_bitscan();
            pawn_prom_attackers.forward_popbit();
            move_list.add(Move{sq, checker_sq, constants::QUEEN, Move::Type::CAPTURE_PROMOTION});
            move_list.add(Move{sq, checker_sq, constants::KNIGHT, Move::Type::CAPTURE_PROMOTION});
            move_list.add(Move{sq, checker_sq, constants::BISHOP, Move::Type::CAPTURE_PROMOTION});
            move_list.add(Move{sq, checker_sq, constants::ROOK, Move::Type::CAPTURE_PROMOTION});
        }
        attackers &= ~rank7_pawns;
        while (attackers) {
            Square sq = attackers.forward_bitscan();
            attackers.forward_popbit();
            move_list.add(Move{sq, checker_sq, constants::PIECE_TYPE_NONE, Move::Type::CAPTURE});
        }
    }
    inline MoveList check_evasion_move_list() const {
        MoveList move_list;
        Color c = side_to_move();
        Square king_sq = king_square(c);
        Bitboard checkers = checkers_to(c);
        Bitboard non_king_occupancy = occupancy_bb() ^ Bitboard { king_sq };

        Bitboard evasions = lookups::king_attacks(king_sq) & ~color_bb(c);
        Bitboard opp_occupancy = color_bb(!c);
        while (evasions) {
            Square sq = evasions.forward_bitscan();
            evasions.forward_popbit();
            if (!attackers_to(sq, !c, non_king_occupancy)) {
                if (Bitboard{sq} & opp_occupancy) {
                    move_list.add(
                        Move{king_sq, sq, constants::PIECE_TYPE_NONE, Move::Type::CAPTURE});
                } else {
                    move_list.add(
                        Move{king_sq, sq, constants::PIECE_TYPE_NONE, Move::Type::NORMAL});
                }
            }
        }

        if (checkers.popcount() > 1) {
            return move_list;
        }

        generate_checker_capture_moves(move_list);
        generate_checker_block_moves(move_list);
        return move_list;
    }
    inline void generate_quiet_moves(MoveList& move_list) const {
        generate_pawn_quiets(move_list);
        generate_non_pawn_quiets(constants::KNIGHT, move_list);
        generate_non_pawn_quiets(constants::BISHOP, move_list);
        generate_non_pawn_quiets(constants::ROOK, move_list);
        generate_non_pawn_quiets(constants::QUEEN, move_list);
        generate_non_pawn_quiets(constants::KING, move_list);
        generate_castling(move_list);
    }
    inline void generate_capture_moves(MoveList& move_list) const {
        generate_pawn_captures(move_list);
        generate_non_pawn_captures(constants::KNIGHT, move_list);
        generate_non_pawn_captures(constants::BISHOP, move_list);
        generate_non_pawn_captures(constants::ROOK, move_list);
        generate_non_pawn_captures(constants::QUEEN, move_list);
        generate_non_pawn_captures(constants::KING, move_list);
    }
    inline MoveList pseudo_legal_move_list() const {
        MoveList move_list;
        generate_pawn_moves(move_list);
        generate_knight_moves(move_list);
        generate_bishop_moves(move_list);
        generate_rook_moves(move_list);
        generate_queen_moves(move_list);
        generate_king_moves(move_list);
        generate_castling(move_list);
        return move_list;
    }
    inline MoveList legal_move_list() const {
        Color stm = side_to_move();
        MoveList move_list =
            checkers_to(stm) ? check_evasion_move_list() : pseudo_legal_move_list();
        for (auto move = move_list.begin(); move != move_list.end();) {
            if (((pinned_pieces(stm) & Bitboard{move->from_square()}) ||
                 move->from_square() == king_square(stm) ||
                 move->type() == Move::Type::ENPASSANT) &&
                !is_legal_move(*move)) {
                move_list.decrement_size();
                *move = *move_list.end();
            } else {
                ++move;
            }
        }
        return move_list;
    }

    inline bool is_repeat(int times = 1) const {
        hash_type curr_hash = hash();
        int num_keys = ply();
        int count = 0;
        for (int i = num_keys - 2; i >= num_keys - halfmoves(); i -= 2) {
            if (state(i).hash_ == curr_hash) {
                ++count;
                if (count >= times) {
                    return true;
                }
            }
        }
        return false;
    }
    inline int repeat_count() const {
        hash_type curr_hash = hash();
        int num_keys = ply();
        int count = 0;
        for (int i = num_keys - 2; i >= num_keys - halfmoves(); i -= 2) {
            if (state(i).hash_ == curr_hash) {
                ++count;
            }
        }
        return count;
    }

    inline std::string fen() const {
        std::string fen_str;
        for (Rank rank = constants::RANK_8; rank >= constants::RANK_1; --rank) {
            int empty_sq_count = 0;
            for (File file = constants::FILE_A; file <= constants::FILE_H; ++file) {
                Square sq = Square::from(file, rank);
                Piece sq_piece = piece_on(sq);
                if (sq_piece != constants::PIECE_NONE) {
                    if (empty_sq_count) {
                        fen_str += std::to_string(empty_sq_count);
                        empty_sq_count = 0;
                    }
                    fen_str += sq_piece.to_char();
                } else {
                    ++empty_sq_count;
                }
            }
            if (empty_sq_count) {
                fen_str += std::to_string(empty_sq_count);
            }
            if (rank != constants::RANK_1) {
                fen_str += '/';
            }
        }

        fen_str += ' ';
        fen_str += side_to_move().to_char();
        fen_str += ' ';
        fen_str += castling_rights().to_str();
        fen_str += ' ';
        fen_str += enpassant_square().to_str();
        fen_str += ' ';
        fen_str += std::to_string(halfmoves());
        fen_str += ' ';
        fen_str += std::to_string(fullmoves());

        return fen_str;
    }
    inline void display_raw(std::ostream& ostream = std::cout) const {
        ostream << "Pawn\n";
        ostream << piece_type_bb(constants::PAWN) << "\n";
        ostream << "Knight\n";
        ostream << piece_type_bb(constants::KNIGHT) << "\n";
        ostream << "Bishop\n";
        ostream << piece_type_bb(constants::BISHOP) << "\n";
        ostream << "Rook\n";
        ostream << piece_type_bb(constants::ROOK) << "\n";
        ostream << "Queen\n";
        ostream << piece_type_bb(constants::QUEEN) << "\n";
        ostream << "King\n";
        ostream << piece_type_bb(constants::KING) << "\n";
        ostream << "White\n";
        ostream << color_bb(constants::WHITE) << "\n";
        ostream << "Black\n";
        ostream << color_bb(constants::BLACK) << "\n";
        ostream << "Side to move: " << side_to_move() << "\n";
        ostream << "Castling rights: " << castling_rights() << "\n";
        ostream << "Enpassant square: " << enpassant_square() << "\n";
        ostream << "Halfmoves: " << halfmoves() << "\n";
        ostream << "Fullmoves: " << fullmoves() << "\n";
        ostream << "Previous move: " << previous_move() << "\n";
        ostream << "Previously captured: " << previously_captured_piece() << "\n";
        ostream << "Ply: " << ply() << "\n";
        ostream << "\n";
    }
    inline void display(std::ostream& ostream = std::cout) const {
        for (Square square : constants::SQUARES) {
            int sq = square.value();
            if (sq && !(sq & 7)) {
                ostream << "\n";
            }
            Piece piece = piece_on(sq ^ 56);
            if (piece.type() == constants::PIECE_TYPE_NONE ||
                piece.color() == constants::COLOR_NONE) {
                ostream << "- ";
            } else {
                ostream << piece << " ";
            }
        }
        ostream << "\n";
    }

  protected:
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
    inline const std::array<State, 256>& history() const { return history_; }
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

  private:
    Bitboard piece_type_bb_[constants::NUM_PIECE_TYPES];
    Bitboard color_bb_[constants::NUM_COLORS];
    Color side_to_move_;
    int fullmoves_;
    int ply_;
    std::array<State, 256> history_;
};

namespace constants {

static std::string STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

}

} // namespace libchess

#endif // LIBCHESS_POSITION_H
