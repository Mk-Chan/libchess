#ifndef LIBCHESS_ATTACKS_H
#define LIBCHESS_ATTACKS_H

namespace libchess {

inline Bitboard Position::checkers_to(Color to_color) const {
    return attackers_to(piece_type_bb(constants::KING, to_color).forward_bitscan(), !to_color);
}

inline Bitboard Position::attackers_to(Square square) const {
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

inline Bitboard Position::attackers_to(Square square, Color attacker_color) const {
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

inline Bitboard Position::attackers_to(Square square, Color attacker_color,
                                       Bitboard occupancy) const {
    Bitboard attackers;
    attackers |= lookups::pawn_attacks(square, !attacker_color) &
                 piece_type_bb(constants::PAWN, attacker_color);
    for (PieceType pt = constants::KNIGHT; pt <= constants::KING; ++pt) {
        attackers |= lookups::non_pawn_piece_type_attacks(pt, square, occupancy) &
                     piece_type_bb(pt, attacker_color);
    }
    return attackers;
}

inline Bitboard Position::pinned_pieces(Color to_color) const {
    Bitboard pinned_bb;
    Square king_sq = king_square(to_color);
    Bitboard pinners_bb = ((piece_type_bb(constants::QUEEN) | piece_type_bb(constants::ROOK)) &
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

} // namespace libchess

#endif // LIBCHESS_ATTACKS_H
