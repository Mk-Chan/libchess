#ifndef LIBCHESS_GETTERS_H
#define LIBCHESS_GETTERS_H

namespace libchess {

inline Bitboard Position::piece_type_bb(PieceType piece_type) const {
    return piece_type_bb_[piece_type.value()];
}

inline Bitboard Position::piece_type_bb(PieceType piece_type, Color color) const {
    return piece_type_bb_[piece_type.value()] & color_bb(color);
}

inline Bitboard Position::color_bb(Color color) const { return color_bb_[color.value()]; }

inline Bitboard Position::occupancy_bb() const {
    return color_bb(constants::WHITE) | color_bb(constants::BLACK);
}

inline Color Position::side_to_move() const { return side_to_move_; }

inline CastlingRights Position::castling_rights() const { return history_[ply_].castling_rights_; }

inline Square Position::enpassant_square() const { return history_[ply_].enpassant_square_; }

inline int Position::halfmoves() const { return history_[ply_].halfmoves_; }

inline int Position::fullmoves() const { return fullmoves_; }

inline Move Position::previous_move() const { return history_[ply_].previous_move_; }

inline PieceType Position::previously_captured_piece() const { return history_[ply_].captured_pt_; }

inline Position::hash_type Position::hash() const { return history_[ply_].hash_; }

inline Square Position::king_square(Color color) const {
    return piece_type_bb(constants::KING, color).forward_bitscan();
}

inline PieceType Position::piece_type_on(Square square) const {
    for (PieceType piece_type : constants::PIECE_TYPES) {
        if (piece_type_bb(piece_type) & Bitboard{square}) {
            return piece_type;
        }
    }
    return constants::PIECE_TYPE_NONE;
}

inline Color Position::color_of(Square square) const {
    for (Color color : constants::COLORS) {
        if (color_bb(color) & Bitboard{square}) {
            return color;
        }
    }
    return constants::COLOR_NONE;
}

inline Piece Position::piece_on(Square square) const {
    return Piece{piece_type_on(square), color_of(square)};
}

inline bool Position::in_check() const { return checkers_to(side_to_move()) != 0; }

inline bool Position::is_repeat(int times) const {
    hash_type curr_hash = hash();
    int num_keys = std::max(0, ply() - halfmoves());
    int count = 0;
    for (int i = ply() - 2; i >= num_keys; i -= 2) {
        if (state(i).hash_ == curr_hash) {
            ++count;
            if (count >= times) {
                return true;
            }
        }
    }
    return false;
}

inline int Position::repeat_count() const {
    hash_type curr_hash = hash();
    int num_keys = std::max(0, ply() - halfmoves());
    int count = 0;
    for (int i = ply() - 2; i >= num_keys; i -= 2) {
        if (state(i).hash_ == curr_hash) {
            ++count;
        }
    }
    return count;
}

inline std::string Position::fen() const {
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

} // namespace libchess

#endif // LIBCHESS_GETTERS_H
