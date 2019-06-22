#include "catch2/catch.hpp"

#include "../Move.h"

using namespace libchess;
using namespace constants;

TEST_CASE("Unknown Move Type", "[Move]") {
    Move e2e3 = Move{E2, E3};
    REQUIRE(e2e3.from_square() == E2);
    REQUIRE(e2e3.to_square() == E3);
    REQUIRE(!e2e3.promotion_piece_type());
    REQUIRE(e2e3.type() == Move::Type::NONE);
}

TEST_CASE("Normal Move Test", "[Move]") {
    Move e2e3 = Move{E2, E3, Move::Type::NORMAL};
    REQUIRE(e2e3.from_square() == E2);
    REQUIRE(e2e3.to_square() == E3);
    REQUIRE(!e2e3.promotion_piece_type());
    REQUIRE(e2e3.type() == Move::Type::NORMAL);
}

TEST_CASE("Capture Move Test", "[Move]") {
    Move e2d3 = Move{E2, D3, Move::Type::CAPTURE};
    REQUIRE(e2d3.from_square() == E2);
    REQUIRE(e2d3.to_square() == D3);
    REQUIRE(!e2d3.promotion_piece_type());
    REQUIRE(e2d3.type() == Move::Type::CAPTURE);
}

TEST_CASE("Promotion Move Test", "[Move]") {
    Move e7e8b = Move{E7, E8, BISHOP, Move::Type::PROMOTION};
    REQUIRE(e7e8b.from_square() == E7);
    REQUIRE(e7e8b.to_square() == E8);
    REQUIRE(e7e8b.type() == Move::Type::PROMOTION);
    REQUIRE(e7e8b.promotion_piece_type() == BISHOP);
}

TEST_CASE("Capture Promotion Move Test", "[Move]") {
    Move e7d8n{E7, E8, KNIGHT, Move::Type::CAPTURE_PROMOTION};
    REQUIRE(e7d8n.from_square() == E7);
    REQUIRE(e7d8n.to_square() == E8);
    REQUIRE(e7d8n.type() == Move::Type::CAPTURE_PROMOTION);
    REQUIRE(e7d8n.promotion_piece_type() == KNIGHT);
}

TEST_CASE("Move parsing test", "[Move]") {
    REQUIRE(*Move::from("e2e4") == Move{E2, E4});
    REQUIRE(*Move::from("e2e3") == Move{E2, E3});
    REQUIRE(*Move::from("g1f3") == Move{G1, F3});
    REQUIRE(*Move::from("e7e8q") == Move{E7, E8, QUEEN});
    REQUIRE(*Move::from("e2d1n") == Move{E2, D1, KNIGHT});
    REQUIRE(!Move::from("abcde"));
    REQUIRE(!Move::from("b1c3e"));
}
