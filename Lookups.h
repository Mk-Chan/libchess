#ifndef LIBCHESS_LOOKUPS_H
#define LIBCHESS_LOOKUPS_H

#include <array>

#include "Bitboard.h"
#include "Color.h"
#include "PieceType.h"

namespace libchess::lookups {

constexpr inline Bitboard RANK_1_MASK{std::uint64_t(0xff)};
constexpr inline Bitboard RANK_2_MASK{std::uint64_t(0xff00)};
constexpr inline Bitboard RANK_3_MASK{std::uint64_t(0xff0000)};
constexpr inline Bitboard RANK_4_MASK{std::uint64_t(0xff000000)};
constexpr inline Bitboard RANK_5_MASK{std::uint64_t(0xff00000000)};
constexpr inline Bitboard RANK_6_MASK{std::uint64_t(0xff0000000000)};
constexpr inline Bitboard RANK_7_MASK{std::uint64_t(0xff000000000000)};
constexpr inline Bitboard RANK_8_MASK{std::uint64_t(0xff00000000000000)};
constexpr inline Bitboard FILE_A_MASK{std::uint64_t(0x0101010101010101)};
constexpr inline Bitboard FILE_B_MASK{std::uint64_t(0x0202020202020202)};
constexpr inline Bitboard FILE_C_MASK{std::uint64_t(0x0404040404040404)};
constexpr inline Bitboard FILE_D_MASK{std::uint64_t(0x0808080808080808)};
constexpr inline Bitboard FILE_E_MASK{std::uint64_t(0x1010101010101010)};
constexpr inline Bitboard FILE_F_MASK{std::uint64_t(0x2020202020202020)};
constexpr inline Bitboard FILE_G_MASK{std::uint64_t(0x4040404040404040)};
constexpr inline Bitboard FILE_H_MASK{std::uint64_t(0x8080808080808080)};

namespace init {

constexpr inline std::array<Bitboard, 64> north() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H7; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq + 8; atk_sq <= constants::H8; atk_sq = atk_sq + 8) {
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> south() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A2; sq <= constants::H8; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq - 8; atk_sq >= constants::A1; atk_sq = atk_sq - 8) {
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> east() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::G8; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq + 1; atk_sq <= constants::H8; atk_sq = atk_sq + 1) {
            if (Bitboard{atk_sq} & FILE_A_MASK) {
                break;
            }
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> west() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::B1; sq <= constants::H8; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq - 1; atk_sq >= constants::A1; atk_sq = atk_sq - 1) {
            if (Bitboard{atk_sq} & FILE_H_MASK) {
                break;
            }
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> northwest() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H7; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq + 7; atk_sq <= constants::H8; atk_sq = atk_sq + 7) {
            if (Bitboard{atk_sq} & FILE_H_MASK) {
                break;
            }
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> southwest() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::B2; sq <= constants::H8; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq - 9; atk_sq >= constants::A1; atk_sq = atk_sq - 9) {
            if (Bitboard{atk_sq} & FILE_H_MASK) {
                break;
            }
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> northeast() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::G7; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq + 9; atk_sq <= constants::H8; atk_sq = atk_sq + 9) {
            if (Bitboard{atk_sq} & FILE_A_MASK) {
                break;
            }
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> southeast() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A2; sq <= constants::H8; ++sq) {
        Bitboard bb;
        for (Square atk_sq = sq - 7; atk_sq >= constants::A1; atk_sq = atk_sq - 7) {
            if (Bitboard{atk_sq} & FILE_A_MASK) {
                break;
            }
            bb |= Bitboard{atk_sq};
        }
        attacks[sq] = bb;
    }
    return attacks;
}
} // namespace init

// Direction bitboards
constexpr inline std::array<Bitboard, 64> NORTH = init::north();
constexpr inline std::array<Bitboard, 64> SOUTH = init::south();
constexpr inline std::array<Bitboard, 64> EAST = init::east();
constexpr inline std::array<Bitboard, 64> WEST = init::west();
constexpr inline std::array<Bitboard, 64> NORTHWEST = init::northwest();
constexpr inline std::array<Bitboard, 64> SOUTHWEST = init::southwest();
constexpr inline std::array<Bitboard, 64> NORTHEAST = init::northeast();
constexpr inline std::array<Bitboard, 64> SOUTHEAST = init::southeast();

constexpr inline Bitboard north(Square square) { return NORTH[square]; }
constexpr inline Bitboard south(Square square) { return SOUTH[square]; }
constexpr inline Bitboard east(Square square) { return EAST[square]; }
constexpr inline Bitboard west(Square square) { return WEST[square]; }
constexpr inline Bitboard northwest(Square square) { return NORTHWEST[square]; }
constexpr inline Bitboard southwest(Square square) { return SOUTHWEST[square]; }
constexpr inline Bitboard northeast(Square square) { return NORTHEAST[square]; }
constexpr inline Bitboard southeast(Square square) { return SOUTHEAST[square]; }

namespace init {

constexpr inline std::array<std::array<libchess::Bitboard, 64>, 2> pawn_attacks() {
    std::array<std::array<libchess::Bitboard, 64>, 2> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        if (sq <= constants::H7) {
            attacks[constants::WHITE][sq] |= Bitboard{sq + 7} & ~FILE_H_MASK;
        }
        if (sq <= constants::G7) {
            attacks[constants::WHITE][sq] |= Bitboard{sq + 9} & ~FILE_A_MASK;
        }
        if (sq >= constants::A2) {
            attacks[constants::BLACK][sq] |= Bitboard{sq - 7} & ~FILE_A_MASK;
        }
        if (sq >= constants::B2) {
            attacks[constants::BLACK][sq] |= Bitboard{sq - 9} & ~FILE_H_MASK;
        }
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> knight_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        if (sq <= constants::G6) {
            attacks[sq] |= Bitboard{sq + 17} & ~FILE_A_MASK;
        }
        if (sq <= constants::H6) {
            attacks[sq] |= Bitboard{sq + 15} & ~FILE_H_MASK;
        }
        if (sq >= constants::B3) {
            attacks[sq] |= Bitboard{sq - 17} & ~FILE_H_MASK;
        }
        if (sq >= constants::A3) {
            attacks[sq] |= Bitboard{sq - 15} & ~FILE_A_MASK;
        }
        if (sq <= constants::F7) {
            attacks[sq] |= Bitboard{sq + 10} & ~(FILE_A_MASK | FILE_B_MASK);
        }
        if (sq <= constants::H7) {
            attacks[sq] |= Bitboard{sq + 6} & ~(FILE_H_MASK | FILE_G_MASK);
        }
        if (sq >= constants::C2) {
            attacks[sq] |= Bitboard{sq - 10} & ~(FILE_H_MASK | FILE_G_MASK);
        }
        if (sq >= constants::A2) {
            attacks[sq] |= Bitboard{sq - 6} & ~(FILE_A_MASK | FILE_B_MASK);
        }
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> king_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        if (sq <= constants::G7) {
            attacks[sq] |= Bitboard{sq + 9} & ~FILE_A_MASK;
        }
        if (sq <= constants::H7) {
            attacks[sq] |= Bitboard{sq + 8};
            attacks[sq] |= Bitboard{sq + 7} & ~FILE_H_MASK;
        }
        if (sq <= constants::G8) {
            attacks[sq] |= Bitboard{sq + 1} & ~FILE_A_MASK;
        }
        if (sq >= constants::B1) {
            attacks[sq] |= Bitboard{sq - 1} & ~FILE_H_MASK;
        }
        if (sq >= constants::A2) {
            attacks[sq] |= Bitboard{sq - 7} & ~FILE_A_MASK;
            attacks[sq] |= Bitboard{sq - 8};
        }
        if (sq >= constants::B2) {
            attacks[sq] |= Bitboard{sq - 9} & ~FILE_H_MASK;
        }
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> bishop_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        attacks[sq] = lookups::northeast(sq) | lookups::southeast(sq) | lookups::southwest(sq) |
                      lookups::northwest(sq);
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> rook_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        attacks[sq] =
            lookups::north(sq) | lookups::east(sq) | lookups::south(sq) | lookups::west(sq);
    }
    return attacks;
}

constexpr inline std::array<Bitboard, 64> queen_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        attacks[sq] = lookups::north(sq) | lookups::east(sq) | lookups::south(sq) |
                      lookups::west(sq) | lookups::northwest(sq) | lookups::northeast(sq) |
                      lookups::southwest(sq) | lookups::southeast(sq);
    }
    return attacks;
}

} // namespace init

// Piece attack bitboards
constexpr inline std::array<std::array<Bitboard, 64>, 2> PAWN_ATTACKS = init::pawn_attacks();
constexpr inline std::array<Bitboard, 64> KNIGHT_ATTACKS = init::knight_attacks();
constexpr inline std::array<Bitboard, 64> KING_ATTACKS = init::king_attacks();
constexpr inline std::array<Bitboard, 64> BISHOP_ATTACKS = init::bishop_attacks();
constexpr inline std::array<Bitboard, 64> ROOK_ATTACKS = init::rook_attacks();
constexpr inline std::array<Bitboard, 64> QUEEN_ATTACKS = init::queen_attacks();

constexpr inline Bitboard pawn_attacks(Square square, Color color) {
    return PAWN_ATTACKS[color][square];
}
constexpr inline Bitboard knight_attacks(Square square, Bitboard = 0) {
    return KNIGHT_ATTACKS[square];
}
constexpr inline Bitboard king_attacks(Square square, Bitboard = 0) { return KING_ATTACKS[square]; }
constexpr inline Bitboard bishop_attacks(Square square) { return BISHOP_ATTACKS[square]; }
constexpr inline Bitboard rook_attacks(Square square) { return ROOK_ATTACKS[square]; }
constexpr inline Bitboard queen_attacks(Square square) { return QUEEN_ATTACKS[square]; }
constexpr inline Bitboard bishop_attacks(Square square, Bitboard occupancy) {
    Bitboard attacks = bishop_attacks(square);
    Bitboard nw_blockers = (northwest(square) & occupancy) | Bitboard{constants::A8};
    Bitboard ne_blockers = (northeast(square) & occupancy) | Bitboard{constants::H8};
    Bitboard sw_blockers = (southwest(square) & occupancy) | Bitboard{constants::A1};
    Bitboard se_blockers = (southeast(square) & occupancy) | Bitboard{constants::H1};

    attacks ^= northwest(nw_blockers.forward_bitscan());
    attacks ^= northeast(ne_blockers.forward_bitscan());
    attacks ^= southwest(sw_blockers.reverse_bitscan());
    attacks ^= southeast(se_blockers.reverse_bitscan());
    return attacks;
}
constexpr inline Bitboard rook_attacks(Square square, Bitboard occupancy) {
    Bitboard attacks = rook_attacks(square);
    Bitboard n_blockers = (north(square) & occupancy) | Bitboard{constants::H8};
    Bitboard s_blockers = (south(square) & occupancy) | Bitboard{constants::A1};
    Bitboard w_blockers = (west(square) & occupancy) | Bitboard{constants::A1};
    Bitboard e_blockers = (east(square) & occupancy) | Bitboard{constants::H8};

    attacks ^= north(n_blockers.forward_bitscan());
    attacks ^= south(s_blockers.reverse_bitscan());
    attacks ^= west(w_blockers.reverse_bitscan());
    attacks ^= east(e_blockers.forward_bitscan());
    return attacks;
}
constexpr inline Bitboard queen_attacks(Square square, Bitboard occupancy) {
    Bitboard attacks = queen_attacks(square);
    Bitboard nw_blockers = (northwest(square) & occupancy) | Bitboard{constants::A8};
    Bitboard ne_blockers = (northeast(square) & occupancy) | Bitboard{constants::H8};
    Bitboard sw_blockers = (southwest(square) & occupancy) | Bitboard{constants::A1};
    Bitboard se_blockers = (southeast(square) & occupancy) | Bitboard{constants::H1};
    Bitboard n_blockers = (north(square) & occupancy) | Bitboard{constants::H8};
    Bitboard s_blockers = (south(square) & occupancy) | Bitboard{constants::A1};
    Bitboard w_blockers = (west(square) & occupancy) | Bitboard{constants::A1};
    Bitboard e_blockers = (east(square) & occupancy) | Bitboard{constants::H8};

    attacks ^= northwest(nw_blockers.forward_bitscan());
    attacks ^= northeast(ne_blockers.forward_bitscan());
    attacks ^= southwest(sw_blockers.reverse_bitscan());
    attacks ^= southeast(se_blockers.reverse_bitscan());
    attacks ^= north(n_blockers.forward_bitscan());
    attacks ^= south(s_blockers.reverse_bitscan());
    attacks ^= west(w_blockers.reverse_bitscan());
    attacks ^= east(e_blockers.forward_bitscan());
    return attacks;
}
constexpr inline Bitboard non_pawn_piece_type_attacks(PieceType piece_type, Square square,
                                                      Bitboard occupancies = 0) {
    switch (piece_type) {
    case constants::KNIGHT:
        return knight_attacks(square);
    case constants::BISHOP:
        return bishop_attacks(square, occupancies);
    case constants::ROOK:
        return rook_attacks(square, occupancies);
    case constants::QUEEN:
        return queen_attacks(square, occupancies);
    case constants::KING:
        return king_attacks(square);
    default:
        return 0;
    }
}

} // namespace libchess::lookups

#endif // LIBCHESS_LOOKUPS_H
