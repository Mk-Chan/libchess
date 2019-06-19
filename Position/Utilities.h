#ifndef LIBCHESS_UTILITIES_H
#define LIBCHESS_UTILITIES_H

namespace libchess {

inline void Position::display_raw(std::ostream& ostream) const {
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

inline void Position::display(std::ostream& ostream) const {
    for (Square square : constants::SQUARES) {
        int sq = square.value();
        if (sq && !(sq & 7)) {
            ostream << "\n";
        }
        Piece piece = piece_on(sq ^ 56);
        if (piece.type() == constants::PIECE_TYPE_NONE || piece.color() == constants::COLOR_NONE) {
            ostream << "- ";
        } else {
            ostream << piece << " ";
        }
    }
    ostream << "\n";
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

inline std::string Position::uci_line() const {
    std::string result = "position " + start_fen();
    result += " moves";
    for (int p = 1; p <= ply(); ++p) {
        result += " " + state(p).previous_move_.to_str();
    }
    return result;
}

} // namespace libchess

#endif // LIBCHESS_UTILITIES_H
