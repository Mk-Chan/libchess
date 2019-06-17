#ifndef LIBCHESS_UTILITIES_H
#define LIBCHESS_UTILITIES_H

namespace libchess {

void Position::display_raw(std::ostream& ostream) const {
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
void Position::display(std::ostream& ostream) const {
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

} // namespace libchess

#endif // LIBCHESS_UTILITIES_H
