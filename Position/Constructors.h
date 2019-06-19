#ifndef LIBCHESS_CONSTRUCTORS_H
#define LIBCHESS_CONSTRUCTORS_H

namespace libchess {

inline Position::Position() : fullmoves_(1), ply_(0) {}

inline Position::Position(const std::string& fen) : fullmoves_(1), ply_(0) {
    *this = Position::from_fen(fen);
    state_mut_ref().hash_ = calculate_hash();
}

} // namespace libchess

#endif // LIBCHESS_CONSTRUCTORS_H
