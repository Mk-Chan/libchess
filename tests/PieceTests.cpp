#include <catch2/catch_all.hpp>

#include "../Piece.h"

using namespace libchess;
using namespace constants;

TEST_CASE("Piece Tests", "[Piece]") {
    REQUIRE(WHITE_PAWN == Piece::from(PAWN, WHITE));
    REQUIRE(WHITE_KNIGHT == Piece::from(KNIGHT, WHITE));
    REQUIRE(WHITE_BISHOP == Piece::from(BISHOP, WHITE));
    REQUIRE(WHITE_ROOK == Piece::from(ROOK, WHITE));
    REQUIRE(WHITE_QUEEN == Piece::from(QUEEN, WHITE));
    REQUIRE(WHITE_KING == Piece::from(KING, WHITE));
    REQUIRE(BLACK_PAWN == Piece::from(PAWN, BLACK));
    REQUIRE(BLACK_KNIGHT == Piece::from(KNIGHT, BLACK));
    REQUIRE(BLACK_BISHOP == Piece::from(BISHOP, BLACK));
    REQUIRE(BLACK_ROOK == Piece::from(ROOK, BLACK));
    REQUIRE(BLACK_QUEEN == Piece::from(QUEEN, BLACK));
    REQUIRE(BLACK_KING == Piece::from(KING, BLACK));

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
