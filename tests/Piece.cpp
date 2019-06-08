#include <catch2/catch.hpp>

#include "../Piece.h"

using namespace libchess;
using namespace constants;

TEST_CASE("Piece Tests", "[Piece]") {
    REQUIRE(WHITE_PAWN == Piece::from(WHITE, PAWN));
    REQUIRE(WHITE_KNIGHT == Piece::from(WHITE, KNIGHT));
    REQUIRE(WHITE_BISHOP == Piece::from(WHITE, BISHOP));
    REQUIRE(WHITE_ROOK == Piece::from(WHITE, ROOK));
    REQUIRE(WHITE_QUEEN == Piece::from(WHITE, QUEEN));
    REQUIRE(WHITE_KING == Piece::from(WHITE, KING));
    REQUIRE(BLACK_PAWN == Piece::from(BLACK, PAWN));
    REQUIRE(BLACK_KNIGHT == Piece::from(BLACK, KNIGHT));
    REQUIRE(BLACK_BISHOP == Piece::from(BLACK, BISHOP));
    REQUIRE(BLACK_ROOK == Piece::from(BLACK, ROOK));
    REQUIRE(BLACK_QUEEN == Piece::from(BLACK, QUEEN));
    REQUIRE(BLACK_KING == Piece::from(BLACK, KING));

    REQUIRE(WHITE_PAWN.to_char() == 'P');
    REQUIRE(WHITE_KNIGHT.to_char() == 'N');
    REQUIRE(WHITE_BISHOP.to_char() == 'B');
    REQUIRE(WHITE_ROOK.to_char() == 'R');
    REQUIRE(WHITE_QUEEN.to_char() == 'Q');
    REQUIRE(WHITE_KING.to_char() == 'K');
    REQUIRE(BLACK_PAWN.to_char() == 'p');
    REQUIRE(BLACK_KNIGHT.to_char() == 'n');
    REQUIRE(BLACK_BISHOP.to_char() == 'b');
    REQUIRE(BLACK_ROOK.to_char() == 'r');
    REQUIRE(BLACK_QUEEN.to_char() == 'q');
    REQUIRE(BLACK_KING.to_char() == 'k');
}
