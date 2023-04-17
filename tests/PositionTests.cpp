#include <catch2/catch.hpp>

#include "../Position.h"

using namespace libchess;
using namespace constants;

TEST_CASE("Null Move Test", "[Position]") {
    Position pos(STARTPOS_FEN);
    CastlingRights old_castling_rights = pos.castling_rights();
    Color old_stm = pos.side_to_move();
    int old_halfmoves = pos.halfmoves();
    auto old_ep_sq = pos.enpassant_square();
    auto old_prev_move = pos.previous_move();
    Position::hash_type old_hash = pos.hash();

    pos.make_null_move();

    REQUIRE(old_castling_rights == pos.castling_rights());
    REQUIRE(!pos.enpassant_square());
    REQUIRE(pos.side_to_move() == !old_stm);
    REQUIRE(!pos.previous_move());
    REQUIRE(pos.halfmoves() == old_halfmoves + 1);
    REQUIRE(pos.hash() != old_hash);
    REQUIRE(pos.hash() == pos.calculate_hash());

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

    REQUIRE(old_hash == 0xcd6a67da5a29065);

    pos.make_move({E2, E4});
    REQUIRE(pos.hash() != old_hash);
    REQUIRE(pos.hash() == pos.calculate_hash());
    pos.unmake_move();
    REQUIRE(pos.hash() == old_hash);

    pos.make_move({E2, E3});
    REQUIRE(pos.hash() != old_hash);
    REQUIRE(pos.hash() == pos.calculate_hash());
    pos.unmake_move();
    REQUIRE(pos.hash() == old_hash);

    pos.make_move({B1, C3});
    REQUIRE(pos.hash() != old_hash);
    REQUIRE(pos.hash() == pos.calculate_hash());
    pos.make_move({B8, C6});
    REQUIRE(pos.hash() != old_hash);
    REQUIRE(pos.hash() == pos.calculate_hash());
    pos.make_move({C3, B1});
    REQUIRE(pos.hash() != old_hash);
    REQUIRE(pos.hash() == pos.calculate_hash());
    pos.make_move({C6, B8});
    REQUIRE(pos.hash() == old_hash);
    REQUIRE(pos.hash() == pos.calculate_hash());
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
    pos = Position{kiwipete_fen};
    REQUIRE(pos.fen() == kiwipete_fen);
}

TEST_CASE("Flip Test", "[Position]") {
    Position pos{"rnbqkbnr/ppppppp1/8/7p/8/8/PPPPPPPP/RNBQKBN1 w Qkq h7 0 1"};

    pos.vflip();
    REQUIRE(pos.fen() == "rnbqkbn1/pppppppp/8/8/7P/8/PPPPPPP1/RNBQKBNR b KQq h2 0 1");

    pos.vflip();
    REQUIRE(pos.fen() == "rnbqkbnr/ppppppp1/8/7p/8/8/PPPPPPPP/RNBQKBN1 w Qkq h7 0 1");
}

TEST_CASE("UCI Position Line Test", "[Position]") {
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string moves = "e2e4 e7e5 g1f3 b8c6";
    std::string line = "position " + fen + " moves " + moves;
    auto pos = Position::from_uci_position_line(line);
    if (!pos.has_value()) {
        FAIL("Invalid Position from line: " + line);
    }
    std::string expected_fen = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3";
    REQUIRE(pos->fen() == expected_fen);
    REQUIRE(pos->start_fen() == fen);
    REQUIRE(pos->uci_line() == line);
}

TEST_CASE("Smallest Capture Move Test Pawn", "[Position]") {
    std::string fen = "7k/4K3/1Q2p1R1/5P2/2BN4/8/4R3/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{F5, E6, Move::Type::CAPTURE};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Knight", "[Position]") {
    std::string fen = "7k/4K3/1Q2p1R1/8/2BN1N2/8/4R3/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{D4, E6, Move::Type::CAPTURE};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Bishop", "[Position]") {
    std::string fen = "7k/4K3/1Q2p1R1/8/2B5/8/4R3/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{C4, E6, Move::Type::CAPTURE};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Rook 1", "[Position]") {
    std::string fen = "7k/4K3/1Q2p1R1/8/8/8/4R3/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{E2, E6, Move::Type::CAPTURE};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Rook 2", "[Position]") {
    std::string fen = "7k/4K3/1Q2p1R1/8/8/8/8/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{G6, E6, Move::Type::CAPTURE};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Queen", "[Position]") {
    std::string fen = "7k/4K3/1Q2p3/8/8/8/8/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{B6, E6, Move::Type::CAPTURE};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test King", "[Position]") {
    std::string fen = "7k/4K3/4p3/8/8/8/8/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{E7, E6, Move::Type::CAPTURE};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Enpassant", "[Position]") {
    std::string fen = "7k/8/3K4/3Pp3/8/8/8/8 w e6 - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E6);
    REQUIRE(actual_move);

    Move expected_move = Move{D5, E6, Move::Type::ENPASSANT};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Promotion White", "[Position]") {
    std::string fen = "4n2k/3K1P2/8/8/8/8/8/8 w - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E8);
    REQUIRE(actual_move);

    Move expected_move = Move{F7, E8, QUEEN, Move::Type::CAPTURE_PROMOTION};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("Smallest Capture Move Test Promotion Black", "[Position]") {
    std::string fen = "7K/8/8/8/8/8/3k1p2/4N3 b - - 0 1";
    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    auto actual_move = pos->smallest_capture_move_to(E1);
    REQUIRE(actual_move);

    Move expected_move = Move{F2, E1, QUEEN, Move::Type::CAPTURE_PROMOTION};
    REQUIRE(*actual_move == expected_move);
}

TEST_CASE("SEE Test KNKP", "[Position]") {
    std::string fen = "7k/8/4p3/8/5N2/8/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_to(E6, {100, 300, 300, 500, 900, 0});
    int expected_value = 100;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Test KNKNP", "[Position]") {
    std::string fen = "7k/2n5/4p3/8/5N2/8/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_to(E6, {100, 300, 300, 500, 900, 0});
    int expected_value = 0;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Test KBNKNP", "[Position]") {
    std::string fen = "7k/2n5/4p3/8/5N2/1B6/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_to(E6, {100, 300, 300, 500, 900, 0});
    int expected_value = 100;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Test KBNKQNP", "[Position]") {
    std::string fen = "4q2k/2n5/4p3/8/5N2/1B6/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_to(E6, {100, 300, 300, 500, 900, 0});
    int expected_value = 0;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Move Test KBNPKQNP Pawn Capture", "[Position]") {
    std::string fen = "4q2k/2n5/4p3/5P2/5N2/1B6/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_for(Move{F5, E6}, {100, 300, 300, 500, 900, 0});
    int expected_value = 100;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Move Test KBNPKQNP Knight Capture", "[Position]") {
    std::string fen = "4q2k/2n5/4p3/5P2/5N2/1B6/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_for(Move{F4, E6}, {100, 300, 300, 500, 900, 0});
    int expected_value = 100;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Move Test KBNPKNPP Pawn Capture", "[Position]") {
    std::string fen = "7k/2np4/4p3/5P2/5N2/1B6/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_for(Move{F5, E6}, {100, 300, 300, 500, 900, 0});
    int expected_value = 100;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Move Test KBNPKNPP Knight Capture", "[Position]") {
    std::string fen = "7k/2np4/4p3/5P2/5N2/1B6/8/K7 w - - 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_for(Move{F4, E6}, {100, 300, 300, 500, 900, 0});
    int expected_value = 0;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Move Test Enpassant", "[Position]") {
    std::string fen = "7k/2np4/8/4pP2/5N2/1B6/8/K7 w - e6 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_for(Move{F5, E6}, {100, 300, 300, 500, 900, 0});
    int expected_value = 100;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("SEE Move Test Promotion Capture", "[Position]") {
    std::string fen = "4n2k/5P2/3N4/4p3/8/1B6/8/K7 w - e6 0 1";

    auto pos = Position::from_fen(fen);
    if (!pos) {
        FAIL("Invalid Position from FEN: " + fen);
    }

    int actual_value = pos->see_for(Move{F7, E8, QUEEN}, {100, 300, 300, 500, 900, 0});
    int expected_value = 1100;

    REQUIRE(actual_value == expected_value);
}

TEST_CASE("Fivefold Repetition Legal Movegen Test", "[Position]") {
    Position pos{constants::STARTPOS_FEN};
    for (int i = 0; i < 4; ++i) {
        pos.make_move(Move{G1, F3});
        pos.make_move(Move{G8, F6});
        pos.make_move(Move{F3, G1});
        pos.make_move(Move{F6, G8});
    }

    REQUIRE(pos.is_repeat(4));
    REQUIRE(pos.repeat_count() == 4);
    REQUIRE(pos.legal_move_list().empty());
}
