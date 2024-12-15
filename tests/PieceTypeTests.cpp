#include <catch2/catch_all.hpp>

#include "../PieceType.h"

using namespace libchess;
using namespace constants;

TEST_CASE("PieceType Tests", "[PieceType]") {
    REQUIRE(PAWN == PieceType::from('p'));
    REQUIRE(PAWN == PieceType::from('P'));
    REQUIRE(KNIGHT == PieceType::from('n'));
    REQUIRE(KNIGHT == PieceType::from('N'));
    REQUIRE(BISHOP == PieceType::from('b'));
    REQUIRE(BISHOP == PieceType::from('B'));
    REQUIRE(ROOK == PieceType::from('r'));
    REQUIRE(ROOK == PieceType::from('R'));
    REQUIRE(QUEEN == PieceType::from('q'));
    REQUIRE(QUEEN == PieceType::from('Q'));
    REQUIRE(KING == PieceType::from('k'));
    REQUIRE(KING == PieceType::from('K'));

    REQUIRE(PAWN.to_char() == 'p');
    REQUIRE(KNIGHT.to_char() == 'n');
    REQUIRE(BISHOP.to_char() == 'b');
    REQUIRE(ROOK.to_char() == 'r');
    REQUIRE(QUEEN.to_char() == 'q');
    REQUIRE(KING.to_char() == 'k');
}
