#ifndef LIBCHESS_ZOBRIST_H
#define LIBCHESS_ZOBRIST_H

#include <array>

#include "../PieceType.h"
#include "../Square.h"

namespace libchess::zobrist {

namespace init {

constexpr inline std::uint64_t xorshift(std::uint64_t x) {
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return x;
}

constexpr inline std::uint64_t random_u64(int i) {
    std::uint64_t seed = 6364136223846793005ULL;
    std::uint64_t x = seed;
    for (; i > 0; --i) {
        x = xorshift(x);
    }
    return x;
}

constexpr inline std::array<std::array<std::array<std::uint64_t, 64>, 6>, 2> piece_square_keys() {
    int rng_seq = 0;
    std::array<std::array<std::array<std::uint64_t, 64>, 6>, 2> keys{};
    for (Color color : constants::COLORS) {
        for (PieceType piece_type : constants::PIECE_TYPES) {
            for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
                keys[color][piece_type][sq] = random_u64(rng_seq);
                ++rng_seq;
            }
        }
    }
    return keys;
}

constexpr inline std::array<std::uint64_t, 16> castling_rights_keys() {
    int rng_seq = 767;
    std::array<std::uint64_t, 16> keys{};
    for (int i = 0; i < 16; ++i) {
        keys[i] = random_u64(rng_seq);
        ++rng_seq;
    }
    return keys;
}

constexpr inline std::array<std::uint64_t, 64> enpassant_keys() {
    int rng_seq = 783;
    std::array<std::uint64_t, 64> keys{};
    for (Square sq = constants::A1; sq <= constants::H8; ++sq) {
        keys[sq] = random_u64(rng_seq);
        ++rng_seq;
    }
    return keys;
}

constexpr inline std::array<std::uint64_t, 2> side_to_move_keys() {
    return {random_u64(847), random_u64(848)};
}

} // namespace init

constexpr inline std::array<std::array<std::array<std::uint64_t, 64>, 6>, 2> PIECE_SQUARE_KEYS =
    init::piece_square_keys();
constexpr inline std::array<std::uint64_t, 16> CASTLING_RIGHTS_KEYS = init::castling_rights_keys();
constexpr inline std::array<std::uint64_t, 64> ENPASSANT_KEYS = init::enpassant_keys();
constexpr inline std::array<std::uint64_t, 2> SIDE_TO_MOVE_KEYS = init::side_to_move_keys();

constexpr inline std::uint64_t piece_square_key(Square square, PieceType piece_type, Color color) {
    return PIECE_SQUARE_KEYS[color][piece_type][square];
}
constexpr inline std::uint64_t castling_rights_key(CastlingRights castling_rights) {
    return CASTLING_RIGHTS_KEYS[castling_rights.value()];
}
constexpr inline std::uint64_t enpassant_key(Square square) { return ENPASSANT_KEYS[square]; }
constexpr inline std::uint64_t side_to_move_key(Color stm) { return SIDE_TO_MOVE_KEYS[stm]; }

} // namespace libchess::zobrist

#endif // LIBCHESS_ZOBRIST_H
