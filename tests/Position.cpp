#include <catch2/catch.hpp>

#include "../Position.h"
#include "../internal/Zobrist.h"

using namespace libchess;
using namespace constants;

TEST_CASE("Null Move Test", "[Position]") {
    Position pos{STARTPOS_FEN};
    CastlingRights old_castling_rights = pos.castling_rights();
    Color old_stm = pos.side_to_move();
    int old_halfmoves = pos.halfmoves();
    Square old_ep_sq = pos.enpassant_square();
    Move old_prev_move = pos.previous_move();
    Position::hash_type old_hash = pos.hash();

    pos.make_null_move();

    REQUIRE(old_castling_rights == pos.castling_rights());
    REQUIRE(pos.enpassant_square() == SQUARE_NONE);
    REQUIRE(pos.side_to_move() == !old_stm);
    REQUIRE(pos.previous_move() == MOVE_NONE);
    REQUIRE(pos.halfmoves() == old_halfmoves + 1);
    REQUIRE(pos.hash() != old_hash);

    pos.unmake_move();

    REQUIRE(old_castling_rights == pos.castling_rights());
    REQUIRE(pos.enpassant_square() == old_ep_sq);
    REQUIRE(pos.side_to_move() == old_stm);
    REQUIRE(pos.previous_move() == old_prev_move);
    REQUIRE(pos.halfmoves() == old_halfmoves);
    REQUIRE(pos.hash() == old_hash);
}

TEST_CASE("Hash Test", "[Position]") {
    Position pos{STARTPOS_FEN};
    Position::hash_type old_hash = pos.hash();

    pos.make_move({E2, E4});
    REQUIRE(pos.hash() != old_hash);
    pos.unmake_move();
    REQUIRE(pos.hash() == old_hash);
}

TEST_CASE("Repetition Test", "[Position]") {
    Position pos{STARTPOS_FEN};

    REQUIRE(!pos.is_repeat());
    REQUIRE(pos.repeat_count() == 0);
    REQUIRE(pos.halfmoves() == 0);
    pos.make_move({G1, F3});
    REQUIRE(!pos.is_repeat());
    REQUIRE(pos.repeat_count() == 0);
    REQUIRE(pos.halfmoves() == 1);
    pos.make_move({G8, F6});
    REQUIRE(!pos.is_repeat());
    REQUIRE(pos.repeat_count() == 0);
    REQUIRE(pos.halfmoves() == 2);
    pos.make_move({F3, G1});
    REQUIRE(!pos.is_repeat());
    REQUIRE(pos.repeat_count() == 0);
    REQUIRE(pos.halfmoves() == 3);
    pos.make_move({F6, G8});
    REQUIRE(pos.is_repeat());
    REQUIRE(pos.repeat_count() == 1);
    REQUIRE(pos.halfmoves() == 4);

    pos.make_move({G1, F3});
    REQUIRE(pos.is_repeat());
    REQUIRE(pos.repeat_count() == 1);
    REQUIRE(pos.halfmoves() == 5);
    pos.make_move({G8, F6});
    REQUIRE(pos.is_repeat());
    REQUIRE(pos.repeat_count() == 1);
    REQUIRE(pos.halfmoves() == 6);
    pos.make_move({F3, G1});
    REQUIRE(pos.is_repeat());
    REQUIRE(pos.repeat_count() == 1);
    REQUIRE(pos.halfmoves() == 7);
    pos.make_move({F6, G8});
    REQUIRE(pos.is_repeat());
    REQUIRE(pos.repeat_count() == 2);
    REQUIRE(pos.halfmoves() == 8);

    pos.make_move({E2, E3});
    REQUIRE(!pos.is_repeat());
    REQUIRE(pos.repeat_count() == 0);
    REQUIRE(pos.halfmoves() == 0);
}

TEST_CASE("FEN Test", "[Position]") {
    Position pos{STARTPOS_FEN};
    REQUIRE(pos.fen() == STARTPOS_FEN);

    std::string kiwipete_fen =
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
    pos = {kiwipete_fen};
    REQUIRE(pos.fen() == kiwipete_fen);
}

TEST_CASE("UCI Position Line Test", "[Position]") {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string moves = "e2e4 e7e5 g1f3 b8c6";
    std::string line = fen + " moves " + moves;
    auto pos = Position::from_uci_position_line(line);
    if (!pos.has_value()) {
        FAIL("Invalid Position from line: " + line);
    }
    std::string expected_fen = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3";
    REQUIRE(pos->fen() == expected_fen);

    line = "position " + fen + " moves " + moves;
    pos = Position::from_uci_position_line(line);
    if (!pos.has_value()) {
        FAIL("Invalid Position from line: " + line);
    }
    expected_fen = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3";
    REQUIRE(pos->fen() == expected_fen);
}
