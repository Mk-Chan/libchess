#include <catch2/catch_all.hpp>

#include "../Color.h"

using namespace libchess;
using namespace constants;

TEST_CASE("Color Tests", "[Color]") {
    REQUIRE((!WHITE) == BLACK);
    REQUIRE(WHITE != BLACK);

    REQUIRE(Color::from('w') == WHITE);
    REQUIRE(Color::from('b') == BLACK);
    REQUIRE(Color::from('W') == WHITE);
    REQUIRE(Color::from('B') == BLACK);

    REQUIRE(WHITE.to_char() == 'w');
    REQUIRE(BLACK.to_char() == 'b');
}
