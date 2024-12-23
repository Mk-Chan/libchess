#ifndef LIBCHESS_ZOBRIST_H
#define LIBCHESS_ZOBRIST_H

#include <array>

#include "../PieceType.h"
#include "../Square.h"
#include "PolyglotRandoms.h"

namespace libchess::zobrist {

constexpr inline std::uint64_t piece_square_key(Square square, PieceType piece_type, Color color) {
    Piece piece = Piece{piece_type, color};
    int piece_offset = -1;
    for (int i = 0; i < 12; ++i) {
        if (constants::PIECES[i] == piece) {
            piece_offset = i;
            break;
        }
    }
    return polyglot::random_u64[piece_offset * 64 + square.value()];
}
constexpr inline std::uint64_t castling_rights_key(CastlingRights castling_rights) {
    std::uint64_t hash = 0;
    if (castling_rights.is_allowed(constants::WHITE_KINGSIDE)) {
        hash ^= polyglot::random_u64[768 + 0];
    }
    if (castling_rights.is_allowed(constants::WHITE_QUEENSIDE)) {
        hash ^= polyglot::random_u64[768 + 1];
    }
    if (castling_rights.is_allowed(constants::BLACK_KINGSIDE)) {
        hash ^= polyglot::random_u64[768 + 2];
    }
    if (castling_rights.is_allowed(constants::BLACK_QUEENSIDE)) {
        hash ^= polyglot::random_u64[768 + 3];
    }
    return hash;
}
constexpr inline std::uint64_t enpassant_key(Square square) {
    return polyglot::random_u64[772 + square.file().value()];
}
constexpr inline std::uint64_t side_to_move_key(Color stm) {
    return polyglot::random_u64[780 + stm.value()];
}

}  // namespace libchess::zobrist

#endif  // LIBCHESS_ZOBRIST_H
