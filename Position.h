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

namespace libchess {

class Position {
  public:
    inline Position(const std::string& fen) : ply_(0) {
        State& curr_state = state();

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
    }
    constexpr inline Position() : fullmoves_(1), ply_(0) {}

    constexpr inline Bitboard piece_type_bb(PieceType piece_type) const {
        return piece_type_bb_[piece_type.value()];
    }
    constexpr inline Bitboard piece_type_bb(PieceType piece_type, Color color) const {
        return piece_type_bb_[piece_type.value()] & color_bb(color);
    }
    constexpr inline Bitboard color_bb(Color color) const { return color_bb_[color.value()]; }
    constexpr inline Bitboard occupancy_bb() const {
        return color_bb(constants::WHITE) | color_bb(constants::BLACK);
    }
    constexpr inline Color side_to_move() const { return side_to_move_; }
    constexpr inline CastlingRights castling_rights() const {
        return history_[ply_].castling_rights_;
    }
    constexpr inline Square enpassant_square() const { return history_[ply_].enpassant_square_; }
    constexpr inline int halfmoves() const { return history_[ply_].halfmoves_; }
    constexpr inline int fullmoves() const { return fullmoves_; }
    constexpr inline Move previous_move() const { return history_[ply_].previous_move_; }
    constexpr inline PieceType previously_captured_piece() const {
        return history_[ply_].captured_pt_;
    }

    constexpr inline PieceType piece_type_on(Square square) const {
        for (PieceType piece_type : constants::PIECE_TYPES) {
            if (piece_type_bb(piece_type) & Bitboard{square}) {
                return piece_type;
            }
        }
        return constants::PIECE_TYPE_NONE;
    }
    constexpr inline Color color_of(Square square) const {
        for (Color color : constants::COLOR_LIST) {
            if (color_bb(color) & Bitboard{square}) {
                return color;
            }
        }
        return constants::COLOR_NONE;
    }
    constexpr inline Piece piece_on(Square square) const {
        return Piece{piece_type_on(square), color_of(square)};
    }

    constexpr inline void unmake_move() {
        Move move = state().previous_move_;
        Move::Type move_type = state().move_type_;
        PieceType captured_pt = state().captured_pt_;
        --ply_;
        reverse_side_to_move();
        Color stm = side_to_move();
        if (stm == constants::BLACK) {
            --fullmoves_;
        }

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
    constexpr inline void make_move(Move move) {
        Color stm = side_to_move();
        if (stm == constants::BLACK) {
            ++fullmoves_;
        }
        State& prev_state = state();
        ++ply_;
        State& next_state = state();
        next_state.halfmoves_ = halfmoves() + 1;
        next_state.previous_move_ = move;
        next_state.enpassant_square_ = constants::SQUARE_INVALID;

        Square from_square = move.from_square();
        Square to_square = move.to_square();

        next_state.castling_rights_ = CastlingRights{prev_state.castling_rights_.value() &
                                                     castling_spoilers[from_square.value()] &
                                                     castling_spoilers[to_square.value()]};

        PieceType moving_pt = piece_type_on(from_square);
        PieceType captured_pt = piece_type_on(to_square);
        PieceType promotion_pt = move.promotion_piece_type();

        Move::Type move_type = move.type();
        if (move_type == constants::MOVE_TYPE_NONE) {
            move_type = [to_square, from_square, moving_pt, captured_pt, promotion_pt]() {
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
            }();
        }

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
    }

    constexpr inline Bitboard attackers_to(Square square) const {
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
    constexpr inline Bitboard attackers_to(Square square, Color attacker_color) const {
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

    constexpr inline void generate_quiet_promotions(MoveList& move_list) const {
        auto [promotion_candidates, forward_sq] = [this, stm = side_to_move()]() {
            if (stm == constants::WHITE) {
                return std::pair{
                    ((piece_type_bb(constants::PAWN, constants::WHITE) & lookups::RANK_7_MASK)
                     << 8) &
                        ~occupancy_bb(),
                    8};
            } else {
                return std::pair{
                    ((piece_type_bb(constants::PAWN, constants::BLACK) & lookups::RANK_2_MASK) >>
                     8) &
                        ~occupancy_bb(),
                    -8};
            }
        }();
        while (promotion_candidates) {
            Square to_sq = promotion_candidates.forward_bitscan();
            promotion_candidates.forward_popbit();
            move_list.add(Move{to_sq - forward_sq, to_sq, constants::QUEEN, Move::Type::PROMOTION});
            move_list.add(
                Move{to_sq - forward_sq, to_sq, constants::KNIGHT, Move::Type::PROMOTION});
            move_list.add(Move{to_sq - forward_sq, to_sq, constants::ROOK, Move::Type::PROMOTION});
            move_list.add(
                Move{to_sq - forward_sq, to_sq, constants::BISHOP, Move::Type::PROMOTION});
        }
    }
    constexpr inline void generate_capture_promotions(MoveList& move_list) const {
        Bitboard pawn_bb = piece_type_bb(constants::PAWN);
        Color stm = side_to_move();
        pawn_bb &=
            color_bb(stm) & (stm == constants::WHITE ? lookups::RANK_7_MASK : lookups::RANK_2_MASK);
        while (pawn_bb) {
            Square from_sq = pawn_bb.forward_bitscan();
            pawn_bb.forward_popbit();
            Bitboard attacks_bb = lookups::pawn_attacks(from_sq, stm) & color_bb(!stm);
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
    constexpr inline void generate_promotions(MoveList& move_list) const {
        generate_capture_promotions(move_list);
        generate_quiet_promotions(move_list);
    }
    constexpr inline void generate_pawn_quiets(MoveList& move_list) const {
        generate_quiet_promotions(move_list);
        Color stm = side_to_move();
        Bitboard occupancy = occupancy_bb();
        auto [single_push_pawn_bb, double_push_pawn_bb, forward_offset] = [this, stm, occupancy]() {
            if (stm == constants::WHITE) {
                Bitboard single_push_pawn_bb =
                    (piece_type_bb(constants::PAWN, stm) & ~lookups::RANK_7_MASK) << 8 & ~occupancy;
                int forward_offset = 8;
                Bitboard double_push_pawn_bb =
                    ((single_push_pawn_bb & lookups::RANK_3_MASK) << 8) & ~occupancy;
                return std::tuple{single_push_pawn_bb, double_push_pawn_bb, forward_offset};
            } else {
                Bitboard single_push_pawn_bb =
                    (piece_type_bb(constants::PAWN, stm) & ~lookups::RANK_2_MASK) >> 8 & ~occupancy;
                int forward_offset = -8;
                Bitboard double_push_pawn_bb =
                    ((single_push_pawn_bb & lookups::RANK_6_MASK) >> 8) & ~occupancy;
                return std::tuple{single_push_pawn_bb, double_push_pawn_bb, forward_offset};
            }
        }();
        while (double_push_pawn_bb) {
            Square to_sq = double_push_pawn_bb.forward_bitscan();
            double_push_pawn_bb.forward_popbit();
            move_list.add(Move{to_sq - 2 * forward_offset, to_sq, constants::PIECE_TYPE_NONE,
                               Move::Type::DOUBLE_PUSH});
        }
        while (single_push_pawn_bb) {
            Square to_sq = single_push_pawn_bb.forward_bitscan();
            single_push_pawn_bb.forward_popbit();
            move_list.add(Move{to_sq - forward_offset, to_sq, constants::PIECE_TYPE_NONE,
                               Move::Type::NORMAL});
        }
    }
    constexpr inline void generate_pawn_captures(MoveList& move_list) const {
        generate_capture_promotions(move_list);
        Bitboard pawn_bb = piece_type_bb(constants::PAWN);
        Square ep_sq = enpassant_square();
        if (ep_sq != constants::SQUARE_INVALID) {
            Bitboard ep_candidates =
                pawn_bb & color_bb(side_to_move()) & lookups::pawn_attacks(ep_sq, !side_to_move());
            while (ep_candidates) {
                Square sq = ep_candidates.forward_bitscan();
                ep_candidates.forward_popbit();
                move_list.add(Move{sq, ep_sq, constants::PIECE_TYPE_NONE, Move::Type::ENPASSANT});
            }
        }
        Color stm = side_to_move();
        pawn_bb &= color_bb(stm) &
                   ~(stm == constants::WHITE ? lookups::RANK_7_MASK : lookups::RANK_2_MASK);
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
    constexpr inline void generate_pawn_moves(MoveList& move_list) const {
        generate_pawn_captures(move_list);
        generate_pawn_quiets(move_list);
    }
    constexpr inline void generate_knight_moves(MoveList& move_list) const {
        generate_non_pawn_quiets<constants::KNIGHT>(move_list);
        generate_non_pawn_captures<constants::KNIGHT>(move_list);
    }
    constexpr inline void generate_bishop_moves(MoveList& move_list) const {
        generate_non_pawn_quiets<constants::BISHOP>(move_list);
        generate_non_pawn_captures<constants::BISHOP>(move_list);
    }
    constexpr inline void generate_rook_moves(MoveList& move_list) const {
        generate_non_pawn_quiets<constants::ROOK>(move_list);
        generate_non_pawn_captures<constants::ROOK>(move_list);
    }
    constexpr inline void generate_queen_moves(MoveList& move_list) const {
        generate_non_pawn_quiets<constants::QUEEN>(move_list);
        generate_non_pawn_captures<constants::QUEEN>(move_list);
    }
    constexpr inline void generate_king_moves(MoveList& move_list) const {
        generate_non_pawn_quiets<constants::KING>(move_list);
        generate_non_pawn_captures<constants::KING>(move_list);
    }
    constexpr inline void generate_castling(MoveList& move_list) const {
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
    constexpr inline void generate_quiet_moves(MoveList& move_list) const {
        generate_pawn_quiets(move_list);
        generate_non_pawn_quiets<constants::KNIGHT>(move_list);
        generate_non_pawn_quiets<constants::BISHOP>(move_list);
        generate_non_pawn_quiets<constants::ROOK>(move_list);
        generate_non_pawn_quiets<constants::QUEEN>(move_list);
        generate_non_pawn_quiets<constants::KING>(move_list);
        generate_castling(move_list);
    }
    constexpr inline void generate_capture_moves(MoveList& move_list) const {
        generate_pawn_captures(move_list);
        generate_non_pawn_captures<constants::KNIGHT>(move_list);
        generate_non_pawn_captures<constants::BISHOP>(move_list);
        generate_non_pawn_captures<constants::ROOK>(move_list);
        generate_non_pawn_captures<constants::QUEEN>(move_list);
        generate_non_pawn_captures<constants::KING>(move_list);
    }
    constexpr inline MoveList pseudo_legal_move_list() const {
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
        CastlingRights castling_rights_;
        Square enpassant_square_;
        Move previous_move_;
        PieceType captured_pt_;
        Move::Type move_type_ = Move::Type::NORMAL;
        int halfmoves_ = 0;
    };

    constexpr inline int ply() const { return ply_; }
    constexpr inline const std::array<State, 256>& history() const { return history_; }
    constexpr inline State& state() { return history_[ply()]; }
    constexpr inline State& state(int ply) { return history_[ply]; }

    constexpr inline void put_piece(Square square, PieceType piece_type, Color color) {
        Bitboard square_bb = Bitboard{square};
        piece_type_bb_[piece_type.value()] |= square_bb;
        color_bb_[color.value()] |= square_bb;
    }
    constexpr inline void remove_piece(Square square, PieceType piece_type, Color color) {
        Bitboard square_bb = Bitboard{square};
        piece_type_bb_[piece_type.value()] &= ~square_bb;
        color_bb_[color.value()] &= ~square_bb;
    }
    constexpr inline void move_piece(Square from_square, Square to_square, PieceType piece_type,
                                     Color color) {
        Bitboard from_to_sqs_bb = Bitboard{from_square} ^ Bitboard { to_square };
        piece_type_bb_[piece_type.value()] ^= from_to_sqs_bb;
        color_bb_[color.value()] ^= from_to_sqs_bb;
    }
    constexpr inline void reverse_side_to_move() { side_to_move_ = !side_to_move_; }
    template <PieceType::value_type pt>
    constexpr inline void generate_non_pawn_quiets(MoveList& move_list) const {
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
    template <PieceType::value_type pt>
    constexpr inline void generate_non_pawn_captures(MoveList& move_list) const {
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

    // clang-format off
    constexpr static inline int castling_spoilers[64] = {
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
}; // namespace libchess

namespace constants {

inline const std::string STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

}

} // namespace libchess

#endif // LIBCHESS_POSITION_H
