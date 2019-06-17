#ifndef LIBCHESS_CONSTRUCTORS_H
#define LIBCHESS_CONSTRUCTORS_H

namespace libchess {

Position::Position(const std::string& fen) : ply_(0) {
    State& curr_state = state_mut_ref();

    std::stringstream fen_stream{fen};
    std::string fen_part;

    // Piece list
    fen_stream >> fen_part;
    Square current_square = constants::A8;
    for (char c : fen_part) {
        if (c >= '1' && c <= '9') {
            current_square += (c - '0');
        } else if (c == '/') {
            current_square -= 16;
        } else {
            auto piece = Piece::from(c);
            put_piece(current_square, piece.type(), piece.color());
            ++current_square;
        }
    }

    // Side to move
    fen_stream >> fen_part;
    side_to_move_ = Color::from(fen_part[0]);

    // Castling rights
    fen_stream >> fen_part;
    curr_state.castling_rights_ = CastlingRights::from(fen_part);

    // Enpassant square
    fen_stream >> fen_part;
    curr_state.enpassant_square_ = Square::from(fen_part);

    // Halfmoves
    fen_stream >> fen_part;
    const char* fen_part_cstr = fen_part.c_str();
    char* end;
    curr_state.halfmoves_ = std::strtol(fen_part_cstr, &end, 10);

    // Fullmoves
    fen_stream >> fen_part;
    fullmoves_ = std::strtol(fen_part_cstr, &end, 10);

    curr_state.hash_ = calculate_hash();
}

Position::Position() : fullmoves_(1), ply_(0) {}

} // namespace libchess

#endif // LIBCHESS_CONSTRUCTORS_H
