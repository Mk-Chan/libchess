#ifndef LIBCHESS_MOVEGENERATION_H
#define LIBCHESS_MOVEGENERATION_H

namespace libchess {

inline void Position::generate_quiet_promotions(MoveList& move_list, Color stm) const {
    Bitboard promotion_candidates =
        lookups::pawn_shift(piece_type_bb(constants::PAWN, stm) &
                                lookups::relative_rank_mask(constants::RANK_7, stm),
                            stm) &
        ~occupancy_bb();
    while (promotion_candidates) {
        Square to_sq = promotion_candidates.forward_bitscan();
        promotion_candidates.forward_popbit();
        move_list.add(
            Move{lookups::pawn_shift(to_sq, !stm), to_sq, constants::QUEEN, Move::Type::PROMOTION});
        move_list.add(Move{
            lookups::pawn_shift(to_sq, !stm), to_sq, constants::KNIGHT, Move::Type::PROMOTION});
        move_list.add(
            Move{lookups::pawn_shift(to_sq, !stm), to_sq, constants::ROOK, Move::Type::PROMOTION});
        move_list.add(Move{
            lookups::pawn_shift(to_sq, !stm), to_sq, constants::BISHOP, Move::Type::PROMOTION});
    }
}

inline void Position::generate_capture_promotions(MoveList& move_list, Color stm) const {
    Bitboard pawn_bb = piece_type_bb(constants::PAWN);
    pawn_bb &= color_bb(stm) & lookups::relative_rank_mask(constants::RANK_7, stm);
    Bitboard opp_occupancy = color_bb(!stm);
    while (pawn_bb) {
        Square from_sq = pawn_bb.forward_bitscan();
        pawn_bb.forward_popbit();
        Bitboard attacks_bb = lookups::pawn_attacks(from_sq, stm) & opp_occupancy;
        while (attacks_bb) {
            Square to_sq = attacks_bb.forward_bitscan();
            attacks_bb.forward_popbit();
            move_list.add(Move{from_sq, to_sq, constants::QUEEN, Move::Type::CAPTURE_PROMOTION});
            move_list.add(Move{from_sq, to_sq, constants::KNIGHT, Move::Type::CAPTURE_PROMOTION});
            move_list.add(Move{from_sq, to_sq, constants::ROOK, Move::Type::CAPTURE_PROMOTION});
            move_list.add(Move{from_sq, to_sq, constants::BISHOP, Move::Type::CAPTURE_PROMOTION});
        }
    }
}

inline void Position::generate_promotions(MoveList& move_list, Color stm) const {
    generate_capture_promotions(move_list, stm);
    generate_quiet_promotions(move_list, stm);
}

inline void Position::generate_pawn_quiets(MoveList& move_list, Color stm) const {
    generate_quiet_promotions(move_list, stm);
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
        move_list.add(Move{lookups::pawn_shift(to_sq, !stm, 2), to_sq, Move::Type::DOUBLE_PUSH});
    }
    while (single_push_pawn_bb) {
        Square to_sq = single_push_pawn_bb.forward_bitscan();
        single_push_pawn_bb.forward_popbit();
        move_list.add(Move{lookups::pawn_shift(to_sq, !stm), to_sq, Move::Type::NORMAL});
    }
}

inline void Position::generate_pawn_captures(MoveList& move_list, Color stm) const {
    generate_capture_promotions(move_list, stm);
    Bitboard pawn_bb = piece_type_bb(constants::PAWN);
    auto ep_sq = enpassant_square();
    if (ep_sq) {
        Bitboard ep_candidates = pawn_bb & color_bb(stm) & lookups::pawn_attacks(*ep_sq, !stm);
        while (ep_candidates) {
            Square sq = ep_candidates.forward_bitscan();
            ep_candidates.forward_popbit();
            move_list.add(Move{sq, *ep_sq, Move::Type::ENPASSANT});
        }
    }
    pawn_bb &= color_bb(stm) & ~lookups::relative_rank_mask(constants::RANK_7, stm);
    while (pawn_bb) {
        Square from_sq = pawn_bb.forward_bitscan();
        pawn_bb.forward_popbit();
        Bitboard attacks_bb = lookups::pawn_attacks(from_sq, stm) & color_bb(!stm);
        while (attacks_bb) {
            Square to_sq = attacks_bb.forward_bitscan();
            attacks_bb.forward_popbit();
            move_list.add(Move{from_sq, to_sq, Move::Type::CAPTURE});
        }
    }
}

inline void Position::generate_pawn_moves(MoveList& move_list, Color stm) const {
    generate_pawn_captures(move_list, stm);
    generate_pawn_quiets(move_list, stm);
}

inline void Position::generate_non_pawn_quiets(PieceType pt, MoveList& move_list, Color stm) const {
    Bitboard piece_bb = piece_type_bb(pt, stm);
    Bitboard occupancy = occupancy_bb();
    while (piece_bb) {
        Square sq = piece_bb.forward_bitscan();
        piece_bb.forward_popbit();
        Bitboard atks = lookups::non_pawn_piece_type_attacks(pt, sq, occupancy) & ~occupancy;
        while (atks) {
            Square to_sq = atks.forward_bitscan();
            atks.forward_popbit();
            move_list.add(Move{sq, to_sq, Move::Type::NORMAL});
        }
    }
}

inline void Position::generate_non_pawn_captures(PieceType pt,
                                                 MoveList& move_list,
                                                 Color stm) const {
    Bitboard piece_bb = piece_type_bb(pt, stm);
    Bitboard occupancy = occupancy_bb();
    Bitboard opp_occupancy = color_bb(!stm);
    while (piece_bb) {
        Square sq = piece_bb.forward_bitscan();
        piece_bb.forward_popbit();
        Bitboard atks = lookups::non_pawn_piece_type_attacks(pt, sq, occupancy) & opp_occupancy;
        while (atks) {
            Square to_sq = atks.forward_bitscan();
            atks.forward_popbit();
            move_list.add(Move{sq, to_sq, Move::Type::CAPTURE});
        }
    }
}

inline void Position::generate_knight_moves(MoveList& move_list, Color stm) const {
    generate_non_pawn_quiets(constants::KNIGHT, move_list, stm);
    generate_non_pawn_captures(constants::KNIGHT, move_list, stm);
}

inline void Position::generate_bishop_moves(MoveList& move_list, Color stm) const {
    generate_non_pawn_quiets(constants::BISHOP, move_list, stm);
    generate_non_pawn_captures(constants::BISHOP, move_list, stm);
}

inline void Position::generate_rook_moves(MoveList& move_list, Color stm) const {
    generate_non_pawn_quiets(constants::ROOK, move_list, stm);
    generate_non_pawn_captures(constants::ROOK, move_list, stm);
}

inline void Position::generate_queen_moves(MoveList& move_list, Color stm) const {
    generate_non_pawn_quiets(constants::QUEEN, move_list, stm);
    generate_non_pawn_captures(constants::QUEEN, move_list, stm);
}

inline void Position::generate_king_moves(MoveList& move_list, Color stm) const {
    generate_non_pawn_quiets(constants::KING, move_list, stm);
    generate_non_pawn_captures(constants::KING, move_list, stm);
}

inline void Position::generate_castling(MoveList& move_list, Color c) const {
    const int castling_possibilities[2][2] = {
        {constants::WHITE_KINGSIDE.value(), constants::WHITE_QUEENSIDE.value()},
        {constants::BLACK_KINGSIDE.value(), constants::BLACK_QUEENSIDE.value()},
    };
    const Square castling_intermediate_sqs[2][2][2] = {
        {{constants::F1, constants::G1}, {constants::D1, constants::C1}},
        {{constants::F8, constants::G8}, {constants::D8, constants::C8}}};
    const Square castling_king_sqs[2][2][2] = {
        {{constants::E1, constants::G1}, {constants::E1, constants::C1}},
        {{constants::E8, constants::G8}, {constants::E8, constants::C8}}};
    const Bitboard castle_mask[2][2] = {
        {(Bitboard{constants::F1} | Bitboard{constants::G1}),
         (Bitboard{constants::D1} | Bitboard{constants::C1} | Bitboard{constants::B1})},
        {(Bitboard{constants::F8} | Bitboard{constants::G8}),
         (Bitboard{constants::D8} | Bitboard{constants::C8} | Bitboard{constants::B8})}};

    Bitboard occupancy = occupancy_bb();
    if ((castling_possibilities[c][0] & castling_rights().value()) &&
        !(castle_mask[c][0] & occupancy) && !(attackers_to(castling_king_sqs[c][0][0], !c)) &&
        !(attackers_to(castling_intermediate_sqs[c][0][0], !c)) &&
        !(attackers_to(castling_intermediate_sqs[c][0][1], !c))) {
        move_list.add(
            Move{castling_king_sqs[c][0][0], castling_king_sqs[c][0][1], Move::Type::CASTLING});
    }
    if ((castling_possibilities[c][1] & castling_rights().value()) &&
        !(castle_mask[c][1] & occupancy) && !(attackers_to(castling_king_sqs[c][0][0], !c)) &&
        !(attackers_to(castling_intermediate_sqs[c][1][0], !c)) &&
        !(attackers_to(castling_intermediate_sqs[c][1][1], !c))) {
        move_list.add(
            Move{castling_king_sqs[c][1][0], castling_king_sqs[c][1][1], Move::Type::CASTLING});
    }
}

inline void Position::generate_checker_block_moves(MoveList& move_list, Color c) const {
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
        move_list.add(Move{pawn_sq, lookups::pawn_shift(pawn_sq, c, 2), Move::Type::DOUBLE_PUSH});
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
            move_list.add(Move{pawn_sq, target_sq, Move::Type::NORMAL});
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
            move_list.add(Move{atker_sq, sq, Move::Type::NORMAL});
        }
    }
}

inline void Position::generate_checker_capture_moves(MoveList& move_list, Color c) const {
    Bitboard checkers = checkers_to(c);
    if (checkers.popcount() > 1) {
        return;
    }

    Bitboard pawns = piece_type_bb(constants::PAWN, c);
    auto ep_square = enpassant_square();
    if (ep_square && (lookups::pawn_shift(Bitboard{*ep_square}, !c) & checkers)) {
        Bitboard ep_candidates = pawns & lookups::pawn_attacks(*ep_square, !c);
        while (ep_candidates) {
            Square sq = ep_candidates.forward_bitscan();
            ep_candidates.forward_popbit();
            move_list.add(Move{sq, *ep_square, Move::Type::ENPASSANT});
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
        move_list.add(Move{sq, checker_sq, Move::Type::CAPTURE});
    }
}

inline MoveList Position::check_evasion_move_list(Color c) const {
    MoveList move_list;
    Square king_sq = king_square(c);
    Bitboard checkers = checkers_to(c);
    Bitboard non_king_occupancy = occupancy_bb() ^ Bitboard { king_sq };

    Bitboard evasions = lookups::king_attacks(king_sq) & ~color_bb(c);
    Bitboard opp_occupancy = color_bb(!c);
    while (evasions) {
        Square sq = evasions.forward_bitscan();
        evasions.forward_popbit();
        if (!(attackers_to(sq, non_king_occupancy, !c))) {
            if (Bitboard{sq} & opp_occupancy) {
                move_list.add(Move{king_sq, sq, Move::Type::CAPTURE});
            } else {
                move_list.add(Move{king_sq, sq, Move::Type::NORMAL});
            }
        }
    }

    if (checkers.popcount() > 1) {
        return move_list;
    }

    generate_checker_capture_moves(move_list, c);
    generate_checker_block_moves(move_list, c);
    return move_list;
}

inline MoveList Position::check_evasion_move_list() const {
    return check_evasion_move_list(side_to_move());
}

inline void Position::generate_quiet_moves(MoveList& move_list, Color stm) const {
    generate_pawn_quiets(move_list, stm);
    generate_non_pawn_quiets(constants::KNIGHT, move_list, stm);
    generate_non_pawn_quiets(constants::BISHOP, move_list, stm);
    generate_non_pawn_quiets(constants::ROOK, move_list, stm);
    generate_non_pawn_quiets(constants::QUEEN, move_list, stm);
    generate_non_pawn_quiets(constants::KING, move_list, stm);
    generate_castling(move_list, stm);
}

inline void Position::generate_capture_moves(MoveList& move_list, Color stm) const {
    generate_pawn_captures(move_list, stm);
    generate_non_pawn_captures(constants::KNIGHT, move_list, stm);
    generate_non_pawn_captures(constants::BISHOP, move_list, stm);
    generate_non_pawn_captures(constants::ROOK, move_list, stm);
    generate_non_pawn_captures(constants::QUEEN, move_list, stm);
    generate_non_pawn_captures(constants::KING, move_list, stm);
}

inline MoveList Position::pseudo_legal_move_list(Color stm) const {
    MoveList move_list;
    generate_capture_moves(move_list, stm);
    generate_quiet_moves(move_list, stm);
    return move_list;
}

inline MoveList Position::pseudo_legal_move_list() const {
    if (in_check()) {
        return check_evasion_move_list(side_to_move());
    }
    return pseudo_legal_move_list(side_to_move());
}

inline MoveList Position::legal_move_list(Color stm) const {
    MoveList move_list = checkers_to(stm) ? check_evasion_move_list() : pseudo_legal_move_list();
    Bitboard pinned = pinned_pieces_of(stm);
    for (auto move = move_list.begin(); move != move_list.end();) {
        if (((pinned & Bitboard{move->from_square()}) || move->from_square() == king_square(stm) ||
             move->type() == Move::Type::ENPASSANT) &&
            !is_legal_generated_move(*move)) {
            *move = *(move_list.end() - 1);
            move_list.pop_back();
        } else {
            ++move;
        }
    }
    return move_list;
}

inline MoveList Position::legal_move_list() const {
    if (halfmoves() >= 150 || is_repeat(4)) {
        return MoveList{};
    }
    return legal_move_list(side_to_move());
}

}  // namespace libchess

#endif  // LIBCHESS_MOVEGENERATION_H
