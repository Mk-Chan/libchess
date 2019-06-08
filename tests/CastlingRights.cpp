#include "catch2/catch.hpp"

#include "../CastlingRights.h"

using namespace libchess;
using namespace constants;

TEST_CASE("CastlingRights Test All", "[CastlingRights]") {
    CastlingRights castling_rights = CASTLING_RIGHTS_ALL;
    REQUIRE(castling_rights.is_allowed(WHITE_KINGSIDE));
    REQUIRE(castling_rights.is_allowed(WHITE_QUEENSIDE));
    REQUIRE(castling_rights.is_allowed(BLACK_KINGSIDE));
    REQUIRE(castling_rights.is_allowed(BLACK_QUEENSIDE));
}

TEST_CASE("CastlingRights Test Some", "[CastlingRights]") {
    CastlingRights castling_rights{WHITE_KINGSIDE, BLACK_QUEENSIDE};
    REQUIRE(castling_rights.is_allowed(WHITE_KINGSIDE));
    REQUIRE(!castling_rights.is_allowed(WHITE_QUEENSIDE));
    REQUIRE(!castling_rights.is_allowed(BLACK_KINGSIDE));
    REQUIRE(castling_rights.is_allowed(BLACK_QUEENSIDE));
}

TEST_CASE("CastlingRights Test None", "[CastlingRights]") {
    CastlingRights castling_rights;
    REQUIRE(!castling_rights.is_allowed(WHITE_KINGSIDE));
    REQUIRE(!castling_rights.is_allowed(WHITE_QUEENSIDE));
    REQUIRE(!castling_rights.is_allowed(BLACK_KINGSIDE));
    REQUIRE(!castling_rights.is_allowed(BLACK_QUEENSIDE));
}
