#ifndef LIBCHESS_LOOKUPS_H
#define LIBCHESS_LOOKUPS_H

#include <array>
#include <random>

#include "Bitboard.h"
#include "Color.h"
#include "PieceType.h"

namespace libchess::lookups {

static Bitboard RANK_1_MASK{std::uint64_t(0xff)};
static Bitboard RANK_2_MASK{std::uint64_t(0xff00)};
static Bitboard RANK_3_MASK{std::uint64_t(0xff0000)};
static Bitboard RANK_4_MASK{std::uint64_t(0xff000000)};
static Bitboard RANK_5_MASK{std::uint64_t(0xff00000000)};
static Bitboard RANK_6_MASK{std::uint64_t(0xff0000000000)};
static Bitboard RANK_7_MASK{std::uint64_t(0xff000000000000)};
static Bitboard RANK_8_MASK{std::uint64_t(0xff00000000000000)};
static Bitboard FILE_A_MASK{std::uint64_t(0x0101010101010101)};
static Bitboard FILE_B_MASK{std::uint64_t(0x0202020202020202)};
static Bitboard FILE_C_MASK{std::uint64_t(0x0404040404040404)};
static Bitboard FILE_D_MASK{std::uint64_t(0x0808080808080808)};
static Bitboard FILE_E_MASK{std::uint64_t(0x1010101010101010)};
static Bitboard FILE_F_MASK{std::uint64_t(0x2020202020202020)};
static Bitboard FILE_G_MASK{std::uint64_t(0x4040404040404040)};
static Bitboard FILE_H_MASK{std::uint64_t(0x8080808080808080)};

static std::array<Bitboard, 8> RANK_MASK = {RANK_1_MASK,
                                            RANK_2_MASK,
                                            RANK_3_MASK,
                                            RANK_4_MASK,
                                            RANK_5_MASK,
                                            RANK_6_MASK,
                                            RANK_7_MASK,
                                            RANK_8_MASK};
static std::array<Bitboard, 8> FILE_MASK = {FILE_A_MASK,
                                            FILE_B_MASK,
                                            FILE_C_MASK,
                                            FILE_D_MASK,
                                            FILE_E_MASK,
                                            FILE_F_MASK,
                                            FILE_G_MASK,
                                            FILE_H_MASK};

inline Bitboard rank_mask(Rank rank) {
    return RANK_MASK[rank.value()];
}
inline Bitboard file_mask(File file) {
    return FILE_MASK[file.value()];
}

namespace init {

inline std::array<Bitboard, 64> north() {
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

inline std::array<Bitboard, 64> south() {
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

inline std::array<Bitboard, 64> east() {
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

inline std::array<Bitboard, 64> west() {
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

inline std::array<Bitboard, 64> northwest() {
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

inline std::array<Bitboard, 64> southwest() {
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

inline std::array<Bitboard, 64> northeast() {
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

inline std::array<Bitboard, 64> southeast() {
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

inline std::array<std::array<Bitboard, 64>, 64> intervening() {
    std::array<std::array<Bitboard, 64>, 64> intervening_bb{};
    for (Square from = constants::A1; from <= constants::H8; ++from) {
        for (Square to = constants::A1; to <= constants::H8; ++to) {
            if (from == to) {
                continue;
            }
            Square high = to;
            Square low = from;
            if (low > high) {
                high = from;
                low = to;
            }
            if (high.file() == low.file()) {
                for (high -= 8; high > low; high -= 8) {
                    intervening_bb[from][to] |= Bitboard{high};
                }
            }
            if (high.rank() == low.rank()) {
                for (--high; high > low; --high) {
                    intervening_bb[from][to] |= Bitboard{high};
                }
            }
            if (high.file() - low.file() == high.rank() - low.rank()) {
                for (high -= 9; high > low; high -= 9) {
                    intervening_bb[from][to] |= Bitboard{high};
                }
            }
            if (low.file() - high.file() == high.rank() - low.rank()) {
                for (high -= 7; high > low; high -= 7) {
                    intervening_bb[from][to] |= Bitboard{high};
                }
            }
        }
    }
    return intervening_bb;
}

}  // namespace init

// Direction bitboards
static std::array<Bitboard, 64> NORTH = init::north();
static std::array<Bitboard, 64> SOUTH = init::south();
static std::array<Bitboard, 64> EAST = init::east();
static std::array<Bitboard, 64> WEST = init::west();
static std::array<Bitboard, 64> NORTHWEST = init::northwest();
static std::array<Bitboard, 64> SOUTHWEST = init::southwest();
static std::array<Bitboard, 64> NORTHEAST = init::northeast();
static std::array<Bitboard, 64> SOUTHEAST = init::southeast();
static std::array<std::array<Bitboard, 64>, 64> INTERVENING = init::intervening();

static Bitboard north(Square square) {
    return NORTH[square];
}
static Bitboard south(Square square) {
    return SOUTH[square];
}
static Bitboard east(Square square) {
    return EAST[square];
}
static Bitboard west(Square square) {
    return WEST[square];
}
static Bitboard northwest(Square square) {
    return NORTHWEST[square];
}
static Bitboard southwest(Square square) {
    return SOUTHWEST[square];
}
static Bitboard northeast(Square square) {
    return NORTHEAST[square];
}
static Bitboard southeast(Square square) {
    return SOUTHEAST[square];
}
static Bitboard intervening(Square from, Square to) {
    return INTERVENING[from][to];
}

namespace init {

inline std::array<std::array<libchess::Bitboard, 64>, 2> pawn_attacks() {
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

inline std::array<Bitboard, 64> knight_attacks() {
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

inline std::array<Bitboard, 64> king_attacks() {
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

inline std::array<Bitboard, 64> bishop_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        attacks[sq] = lookups::northeast(sq) | lookups::southeast(sq) | lookups::southwest(sq) |
                      lookups::northwest(sq);
    }
    return attacks;
}

inline std::array<Bitboard, 64> rook_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        attacks[sq] =
            lookups::north(sq) | lookups::east(sq) | lookups::south(sq) | lookups::west(sq);
    }
    return attacks;
}

inline std::array<Bitboard, 64> queen_attacks() {
    std::array<Bitboard, 64> attacks{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        attacks[sq] = lookups::north(sq) | lookups::east(sq) | lookups::south(sq) |
                      lookups::west(sq) | lookups::northwest(sq) | lookups::northeast(sq) |
                      lookups::southwest(sq) | lookups::southeast(sq);
    }
    return attacks;
}

}  // namespace init

// Piece attack bitboards
static std::array<std::array<Bitboard, 64>, 2> PAWN_ATTACKS = init::pawn_attacks();
static std::array<Bitboard, 64> KNIGHT_ATTACKS = init::knight_attacks();
static std::array<Bitboard, 64> KING_ATTACKS = init::king_attacks();
static std::array<Bitboard, 64> BISHOP_ATTACKS = init::bishop_attacks();
static std::array<Bitboard, 64> ROOK_ATTACKS = init::rook_attacks();
static std::array<Bitboard, 64> QUEEN_ATTACKS = init::queen_attacks();

inline Bitboard pawn_attacks(Square square, Color color) {
    return PAWN_ATTACKS[color][square];
}
inline Bitboard knight_attacks(Square square, Bitboard = Bitboard{0}) {
    return KNIGHT_ATTACKS[square];
}
inline Bitboard king_attacks(Square square, Bitboard = Bitboard{0}) {
    return KING_ATTACKS[square];
}
inline Bitboard bishop_attacks(Square square) {
    return BISHOP_ATTACKS[square];
}
inline Bitboard rook_attacks(Square square) {
    return ROOK_ATTACKS[square];
}
inline Bitboard queen_attacks(Square square) {
    return QUEEN_ATTACKS[square];
}
inline Bitboard pawn_shift(Bitboard bb, Color c, int times = 1) {
    return c == constants::WHITE ? bb << (8 * times) : bb >> (8 * times);
}
inline Square pawn_shift(Square sq, Color c, int times = 1) {
    return c == constants::WHITE ? sq + (8 * times) : sq - (8 * times);
}
inline Rank relative_rank(Rank rank, Color c) {
    return c == constants::WHITE
               ? rank
               : Rank{static_cast<Rank::value_type>(constants::RANK_8.value() - rank.value())};
}
inline Bitboard relative_rank_mask(Rank rank, Color c) {
    return rank_mask(relative_rank(rank, c));
}

inline Bitboard bishop_attacks_classical(Square square, Bitboard occupancy) {
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
inline Bitboard rook_attacks_classical(Square square, Bitboard occupancy) {
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

class PlainMagicLookup {
   public:
    PlainMagicLookup() : mask_(0), magic_(0) {
    }

    const Bitboard& mask() const {
        return mask_;
    }
    constexpr void mask(const Bitboard& mask) {
        mask_ = mask;
    }
    constexpr const Bitboard& magic() const {
        return magic_;
    }
    constexpr void magic(const Bitboard& magic) {
        magic_ = magic;
    }

    constexpr int magic_rook_attack_index(Bitboard occupancy) const {
        return ((occupancy & mask_) * magic_) >> 52;
    }
    constexpr int magic_bishop_attack_index(Bitboard occupancy) const {
        return ((occupancy & mask_) * magic_) >> 55;
    }

   private:
    Bitboard mask_;
    Bitboard magic_;
};

enum class SlidingPieceType : bool
{
    BISHOP = false,
    ROOK = true
};

template <SlidingPieceType slidingPieceType>
struct MagicAttacksLookup {
    std::array<PlainMagicLookup, 64> magic_lookup;
    std::array<std::array<Bitboard, (slidingPieceType == SlidingPieceType::ROOK ? 4096 : 512)>, 64>
        attacks;
};

namespace init {

inline MagicAttacksLookup<SlidingPieceType::ROOK> rook_magic_attacks_lookup() {
    std::array<PlainMagicLookup, 64> magic_lookup;
    std::array<std::array<Bitboard, 4096>, 64> attacks;
    std::array<Bitboard, 4096> occupancy_combinations;
    std::array<Bitboard, 4096> legal_sliding_attacks;

    std::mt19937_64 rng(std::uint64_t(6254242335641602973));
    for (Square square = constants::A1; square <= constants::H8; ++square) {
        PlainMagicLookup& square_magic = magic_lookup[square];
        Bitboard edges = ((FILE_A_MASK | FILE_H_MASK) & ~file_mask(square.file())) |
                         ((RANK_1_MASK | RANK_8_MASK) & ~rank_mask(square.rank()));
        square_magic.mask(lookups::rook_attacks(square) & ~edges);

        Bitboard mask = square_magic.mask();
        Bitboard current_occupancy{0};
        int occupancy_combination_iterator = 0;
        do {
            occupancy_combinations[occupancy_combination_iterator] = current_occupancy;
            legal_sliding_attacks[occupancy_combination_iterator] =
                rook_attacks_classical(square, current_occupancy);

            ++occupancy_combination_iterator;
            current_occupancy = Bitboard{(current_occupancy - mask) & mask};
        } while (current_occupancy);
        int occupancy_combinations_count = occupancy_combination_iterator;

        std::array<bool, 4096> used_index_set;
        std::fill(used_index_set.begin(), used_index_set.end(), false);
        occupancy_combination_iterator = 0;
        while (occupancy_combination_iterator < occupancy_combinations_count) {
            square_magic.magic(Bitboard{rng() & rng() & rng()});

            occupancy_combination_iterator = 0;
            while (occupancy_combination_iterator < occupancy_combinations_count) {
                int attack_table_index = square_magic.magic_rook_attack_index(
                    occupancy_combinations[occupancy_combination_iterator]);
                if (used_index_set[attack_table_index] &&
                    attacks[square][attack_table_index] !=
                        legal_sliding_attacks[occupancy_combination_iterator]) {
                    std::fill(used_index_set.begin(), used_index_set.end(), false);
                    break;
                }
                attacks[square][attack_table_index] =
                    legal_sliding_attacks[occupancy_combination_iterator];
                used_index_set[attack_table_index] = true;
                ++occupancy_combination_iterator;
            }
        }
    }

    return MagicAttacksLookup<SlidingPieceType::ROOK>{magic_lookup, attacks};
}

inline MagicAttacksLookup<SlidingPieceType::BISHOP> bishop_magic_attacks_lookup() {
    std::array<PlainMagicLookup, 64> magic_lookup;
    std::array<std::array<Bitboard, 512>, 64> attacks;
    std::array<Bitboard, 512> occupancy_combinations;
    std::array<Bitboard, 512> legal_sliding_attacks;

    std::mt19937_64 rng(std::uint64_t(6254242335641602973));
    Bitboard edges = lookups::rook_attacks(constants::A1) | lookups::rook_attacks(constants::H8) |
                     Bitboard{constants::A1} | Bitboard{constants::H8};
    for (Square square = constants::A1; square <= constants::H8; ++square) {
        PlainMagicLookup& square_magic = magic_lookup[square];
        square_magic.mask(lookups::bishop_attacks(square) & ~edges);

        Bitboard mask = square_magic.mask();
        Bitboard current_occupancy{0};
        int occupancy_combination_iterator = 0;
        do {
            occupancy_combinations[occupancy_combination_iterator] = current_occupancy;
            legal_sliding_attacks[occupancy_combination_iterator] =
                bishop_attacks_classical(square, current_occupancy);

            ++occupancy_combination_iterator;
            current_occupancy = Bitboard{(current_occupancy - mask) & mask};
        } while (current_occupancy);
        int occupancy_combinations_count = occupancy_combination_iterator;

        std::array<bool, 512> used_index_set;
        std::fill(used_index_set.begin(), used_index_set.end(), false);
        occupancy_combination_iterator = 0;
        while (occupancy_combination_iterator < occupancy_combinations_count) {
            square_magic.magic(Bitboard{rng() & rng() & rng()});

            occupancy_combination_iterator = 0;
            while (occupancy_combination_iterator < occupancy_combinations_count) {
                int attack_table_index = square_magic.magic_bishop_attack_index(
                    occupancy_combinations[occupancy_combination_iterator]);
                if (used_index_set[attack_table_index] &&
                    attacks[square][attack_table_index] !=
                        legal_sliding_attacks[occupancy_combination_iterator]) {
                    std::fill(used_index_set.begin(), used_index_set.end(), false);
                    break;
                }
                attacks[square][attack_table_index] =
                    legal_sliding_attacks[occupancy_combination_iterator];
                used_index_set[attack_table_index] = true;
                ++occupancy_combination_iterator;
            }
        }
    }

    return MagicAttacksLookup<SlidingPieceType::BISHOP>{magic_lookup, attacks};
}

}  // namespace init

static MagicAttacksLookup<SlidingPieceType::ROOK> rook_magic_attacks_lookup =
    init::rook_magic_attacks_lookup();
static MagicAttacksLookup<SlidingPieceType::BISHOP> bishop_magic_attacks_lookup =
    init::bishop_magic_attacks_lookup();

inline Bitboard rook_attacks(Square square, Bitboard occupancy) {
    return rook_magic_attacks_lookup.attacks[square][rook_magic_attacks_lookup.magic_lookup[square]
                                                         .magic_rook_attack_index(occupancy)];
}
inline Bitboard bishop_attacks(Square square, Bitboard occupancy) {
    return bishop_magic_attacks_lookup
        .attacks[square][bishop_magic_attacks_lookup.magic_lookup[square].magic_bishop_attack_index(
            occupancy)];
}
inline Bitboard queen_attacks(Square square, Bitboard occupancy) {
    return rook_attacks(square, occupancy) | bishop_attacks(square, occupancy);
}
inline Bitboard non_pawn_piece_type_attacks(PieceType piece_type,
                                            Square square,
                                            Bitboard occupancies = Bitboard{0}) {
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
            return Bitboard{0};
    }
}

namespace init {

inline std::array<std::array<Bitboard, 64>, 64> full_ray() {
    std::array<std::array<Bitboard, 64>, 64> full_ray_bb{};
    for (Square from = constants::A1; from <= constants::H8; ++from) {
        for (Square to = constants::A1; to <= constants::H8; ++to) {
            if (from == to) {
                continue;
            }
            Square high = to;
            Square low = from;
            if (low > high) {
                high = from;
                low = to;
            }
            if (high.file() == low.file()) {
                full_ray_bb[from][to] = (lookups::rook_attacks(high) & lookups::rook_attacks(low)) |
                                        Bitboard{from} | Bitboard{to};
            }
            if (high.rank() == low.rank()) {
                full_ray_bb[from][to] = (lookups::rook_attacks(high) & lookups::rook_attacks(low)) |
                                        Bitboard{from} | Bitboard{to};
            }
            if (high.file() - low.file() == high.rank() - low.rank()) {
                full_ray_bb[from][to] =
                    (lookups::bishop_attacks(high) & lookups::bishop_attacks(low)) |
                    Bitboard{from} | Bitboard{to};
            }
            if (low.file() - high.file() == high.rank() - low.rank()) {
                full_ray_bb[from][to] =
                    (lookups::bishop_attacks(high) & lookups::bishop_attacks(low)) |
                    Bitboard{from} | Bitboard{to};
            }
        }
    }
    return full_ray_bb;
}

}  // namespace init

static std::array<std::array<Bitboard, 64>, 64> FULL_RAY = init::full_ray();

inline Bitboard full_ray(Square from, Square to) {
    return FULL_RAY[from][to];
}

}  // namespace libchess::lookups

#endif  // LIBCHESS_LOOKUPS_H
