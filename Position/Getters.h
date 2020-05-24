#ifndef LIBCHESS_GETTERS_H
#define LIBCHESS_GETTERS_H

namespace libchess {

inline Bitboard Position::piece_type_bb(PieceType piece_type) const {
    return piece_type_bb_[piece_type.value()];
}

inline Bitboard Position::piece_type_bb(PieceType piece_type, Color color) const {
    return piece_type_bb_[piece_type.value()] & color_bb(color);
}

inline Bitboard Position::color_bb(Color color) const {
    return color_bb_[color.value()];
}

inline Bitboard Position::occupancy_bb() const {
    return color_bb(constants::WHITE) | color_bb(constants::BLACK);
}

inline Color Position::side_to_move() const {
    return side_to_move_;
}

inline CastlingRights Position::castling_rights() const {
    return history_[ply_].castling_rights_;
}

inline std::optional<Square> Position::enpassant_square() const {
    return history_[ply_].enpassant_square_;
}

inline int Position::halfmoves() const {
    return history_[ply_].halfmoves_;
}

inline int Position::fullmoves() const {
    return fullmoves_;
}

inline std::optional<Move> Position::previous_move() const {
    return history_[ply_].previous_move_;
}

inline std::optional<PieceType> Position::previously_captured_piece() const {
    return history_[ply_].captured_pt_;
}

inline Position::hash_type Position::hash() const {
    return history_[ply_].hash_;
}

inline Position::hash_type Position::pawn_hash() const {
    return history_[ply_].pawn_hash_;
}

inline Square Position::king_square(Color color) const {
    return piece_type_bb(constants::KING, color).forward_bitscan();
}

inline std::optional<PieceType> Position::piece_type_on(Square square) const {
    for (PieceType piece_type : constants::PIECE_TYPES) {
        if (piece_type_bb(piece_type) & Bitboard{square}) {
            return piece_type;
        }
    }
    return std::nullopt;
}

inline std::optional<Color> Position::color_of(Square square) const {
    for (Color c : constants::COLORS) {
        if (color_bb(c) & Bitboard{square}) {
            return c;
        }
    }
    return std::nullopt;
}

inline std::optional<Piece> Position::piece_on(Square square) const {
    return Piece::from(piece_type_on(square), color_of(square));
}

inline bool Position::in_check() const {
    return checkers_to(side_to_move()) != 0;
}

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

inline const std::string& Position::start_fen() const {
    return start_fen_;
}

inline Position::GameState Position::game_state() const {
    if (is_repeat(2)) {
        return GameState::THREEFOLD_REPETITION;
    } else if (halfmoves() >= 100) {
        return GameState::FIFTY_MOVES;
    } else {
        MoveList move_list = legal_move_list();

        if (move_list.empty()) {
            return in_check() ? GameState::CHECKMATE : GameState::STALEMATE;
        }

        return GameState::IN_PROGRESS;
    }
}

inline bool Position::is_legal_generated_move(Move move) const {
    Color c = side_to_move();
    Square from = move.from_square();
    Square king_sq = king_square(c);
    if (move.type() == Move::Type::ENPASSANT) {
        Bitboard ep_bb = Bitboard{*enpassant_square()};
        Bitboard post_ep_occupancy =
            (occupancy_bb() ^ Bitboard{from} ^ lookups::pawn_shift(ep_bb, !c)) | ep_bb;

        return !(lookups::rook_attacks(king_sq, post_ep_occupancy) & color_bb(!c) &
                 (piece_type_bb(constants::QUEEN) | piece_type_bb(constants::ROOK))) &&
               !(lookups::bishop_attacks(king_sq, post_ep_occupancy) & color_bb(!c) &
                 ((piece_type_bb(constants::QUEEN) | piece_type_bb(constants::BISHOP))));
    } else if (from == king_sq) {
        return move.type() == Move::Type::CASTLING ||
               !(attackers_to(move.to_square()) & color_bb(!c));
    } else {
        return !(pinned_pieces_of(c) & Bitboard{from}) ||
               (Bitboard{move.to_square()} & lookups::direction_xray(king_sq, from));
    }
}

inline bool Position::is_legal_move(Move move) const {
    Square from_sq = move.from_square();
    auto piece_opt = piece_on(from_sq);
    if (!piece_opt) {
        return false;
    }

    Piece piece = *piece_opt;
    Color c = piece.color();
    if (c != side_to_move()) {
        return false;
    }

    Square to_sq = move.to_square();
    if (color_of(to_sq) == c) {
        return false;
    }

    PieceType pt = piece.type();
    if (move.promotion_piece_type()) {
        if (pt != constants::PAWN) {
            return false;
        }

        if (lookups::relative_rank(to_sq.rank(), c) != constants::RANK_8) {
            return false;
        }
    }

    bool is_pseudo_legal = false;
    if (pt == constants::KING) {
        MoveList castling_move_list;
        generate_castling(castling_move_list, c);
        if (castling_move_list.contains(move)) {
            is_pseudo_legal = true;
        }
    }

    if (!is_pseudo_legal) {
        auto ep_sq = enpassant_square();
        Bitboard to_sq_bb{to_sq};
        Bitboard opponent_occupancy = color_bb(!c);
        Bitboard stm_occupancy = color_bb(c);
        Bitboard occupancy = stm_occupancy | opponent_occupancy;
        if (pt == constants::PAWN) {
            if (to_sq == lookups::pawn_shift(from_sq, c) && !(occupancy & to_sq_bb)) {
                is_pseudo_legal = true;
            } else if (to_sq == lookups::pawn_shift(from_sq, c, 2) &&
                       !(occupancy & (to_sq_bb | lookups::pawn_shift(to_sq_bb, !c)))) {
                is_pseudo_legal = true;
            } else if (ep_sq && to_sq == *ep_sq &&
                       (lookups::pawn_attacks(from_sq, c) & Bitboard{*ep_sq})) {
                is_pseudo_legal = true;
            } else if (to_sq_bb & lookups::pawn_attacks(from_sq, c) & opponent_occupancy) {
                is_pseudo_legal = true;
            }
        } else if (to_sq_bb & lookups::non_pawn_piece_type_attacks(pt, from_sq, occupancy) &
                   ~stm_occupancy) {
            is_pseudo_legal = true;
        }
    }

    if (!is_pseudo_legal) {
        return false;
    }
    if (!in_check()) {
        return is_legal_generated_move(move);
    }

    MoveList move_list = legal_move_list();
    return move_list.contains(move);
}

}  // namespace libchess

#endif  // LIBCHESS_GETTERS_H
