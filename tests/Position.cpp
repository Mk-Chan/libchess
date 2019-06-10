#include <catch2/catch.hpp>

#include "../Position.h"

using namespace libchess;
using namespace constants;

TEST_CASE("Null Move Test", "[Position]") {
    Position pos{STARTPOS_FEN};
    CastlingRights old_castling_rights = pos.castling_rights();
    Color old_stm = pos.side_to_move();
    int old_halfmoves = pos.halfmoves();
    Square old_ep_sq = pos.enpassant_square();
    Move old_prev_move = pos.previous_move();
    pos.make_null_move();

    REQUIRE(old_castling_rights == pos.castling_rights());
    REQUIRE(pos.enpassant_square() == SQUARE_NONE);
    REQUIRE(pos.side_to_move() == !old_stm);
    REQUIRE(pos.previous_move() == MOVE_NONE);
    REQUIRE(pos.halfmoves() == old_halfmoves + 1);

    pos.unmake_move();

    REQUIRE(old_castling_rights == pos.castling_rights());
    REQUIRE(pos.enpassant_square() == old_ep_sq);
    REQUIRE(pos.side_to_move() == old_stm);
    REQUIRE(pos.previous_move() == old_prev_move);
    REQUIRE(pos.halfmoves() == old_halfmoves);
}
