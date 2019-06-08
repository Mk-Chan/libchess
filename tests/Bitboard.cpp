#include <catch2/catch.hpp>

#include "../Bitboard.h"

using namespace libchess;

TEST_CASE("Bitboard Tests", "[Bitboard]") {
    REQUIRE(Bitboard(2) == 0x4);
    REQUIRE(Bitboard(2u) == 0x4);
    REQUIRE(Bitboard(std::uint64_t(2)) == 0x2);

    REQUIRE((Bitboard(2) << 2) == Bitboard(4));
    REQUIRE((Bitboard(6) >> 2) == Bitboard(4));
    REQUIRE((Bitboard(2) | Bitboard(3)) == 0xc);
    REQUIRE((Bitboard(2) & Bitboard(2)) == 0x4);
    REQUIRE((Bitboard(2) ^ Bitboard(3)) == 0xc);
    REQUIRE((Bitboard(2) ^ Bitboard(2)) == 0);

    REQUIRE((~Bitboard(0)) == 0xffffffffffffffff - 1);
    REQUIRE((!Bitboard()) == true);
    REQUIRE((!Bitboard(0)) == false);
    REQUIRE((!Bitboard(1)) == false);
}
