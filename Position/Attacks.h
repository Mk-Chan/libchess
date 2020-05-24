#ifndef LIBCHESS_ATTACKS_H
#define LIBCHESS_ATTACKS_H

namespace libchess {

inline Bitboard Position::checkers_to(Color c) const {
    return attackers_to(king_square(c), !c);
}

inline Bitboard Position::attackers_to(Square square, Bitboard occupancy) const {
    Bitboard attackers;
    attackers |= lookups::pawn_attacks(square, constants::WHITE) &
                 piece_type_bb(constants::PAWN, constants::BLACK);
    attackers |= lookups::pawn_attacks(square, constants::BLACK) &
                 piece_type_bb(constants::PAWN, constants::WHITE);
    for (PieceType pt = constants::KNIGHT; pt <= constants::KING; ++pt) {
        attackers |=
            lookups::non_pawn_piece_type_attacks(pt, square, occupancy) & piece_type_bb(pt);
    }
    return attackers;
}

inline Bitboard Position::attackers_to(Square square) const {
    return attackers_to(square, occupancy_bb());
}

inline Bitboard Position::attackers_to(Square square, Color c) const {
    return attackers_to(square, occupancy_bb()) & color_bb(c);
}

inline Bitboard Position::attackers_to(Square square, Bitboard occupancy, Color c) const {
    return attackers_to(square, occupancy) & color_bb(c);
}

inline Bitboard Position::attacks_of_piece_on(libchess::Square square) const {
    auto piece = piece_on(square);
    if (!piece) {
        return Bitboard{0};
    }

    if (piece->type() == constants::PAWN) {
        return lookups::pawn_attacks(square, piece->color());
    } else {
        return lookups::non_pawn_piece_type_attacks(piece->type(), square, occupancy_bb());
    }
}

inline Bitboard Position::pinned_pieces_of(Color c) const {
    Bitboard pinned_bb;
    Square king_sq = king_square(c);
    Bitboard pinners_bb = ((piece_type_bb(constants::QUEEN) | piece_type_bb(constants::ROOK)) &
                           color_bb(!c) & lookups::rook_attacks(king_sq)) |
                          ((piece_type_bb(constants::QUEEN) | piece_type_bb(constants::BISHOP)) &
                           color_bb(!c) & lookups::bishop_attacks(king_sq));
    while (pinners_bb) {
        Square sq = pinners_bb.forward_bitscan();
        pinners_bb.forward_popbit();
        Bitboard bb = lookups::intervening(sq, king_sq) & occupancy_bb();
        if (bb.popcount() == 1) {
            pinned_bb ^= bb & color_bb(c);
        }
    }
    return pinned_bb;
}

}  // namespace libchess

#endif  // LIBCHESS_ATTACKS_H
