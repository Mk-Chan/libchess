#include <catch2/catch.hpp>

#include "../UCIService.h"

using namespace libchess;

TEST_CASE("Position Line Test", "[UCIService]") {
    std::string line = "startpos moves e2e4 c7c5 g1f3 d7d6 d2d4";
    std::stringstream line_stream{line};
    auto position_params = UCIService::parse_position_line(line_stream);
    REQUIRE(position_params);
    REQUIRE(position_params->fen() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    REQUIRE(position_params->move_list());
    const std::vector<std::string>& move_list = *position_params->move_list();
    REQUIRE(move_list[0] == "e2e4");
    REQUIRE(move_list[1] == "c7c5");
    REQUIRE(move_list[2] == "g1f3");
    REQUIRE(move_list[3] == "d7d6");
    REQUIRE(move_list[4] == "d2d4");
}

TEST_CASE("Go Line Test Single Parameter", "[UCIService]") {
    std::string line = "movetime 10000";
    std::stringstream line_stream{line};
    auto go_params = UCIService::parse_go_line(line_stream);
    REQUIRE(go_params);
    REQUIRE(go_params->movetime());
    REQUIRE_FALSE((go_params->wtime() || go_params->winc() || go_params->btime() ||
                   go_params->binc() || go_params->depth() || go_params->nodes() ||
                   go_params->infinite() || go_params->ponder() || go_params->searchmoves()));
    REQUIRE(go_params->movetime().value() == 10000);
}

TEST_CASE("Go Line Test Multiple Parameters", "[UCIService]") {
    std::string line = "wtime 32 winc 56 btime 12 binc 54 depth 100 nodes 9991234567899999";
    std::stringstream line_stream{line};
    auto go_params = UCIService::parse_go_line(line_stream);
    REQUIRE(go_params);
    REQUIRE_FALSE((go_params->movetime() || go_params->infinite() || go_params->ponder() ||
                   go_params->searchmoves()));
    REQUIRE(go_params->wtime().value() == 32);
    REQUIRE(go_params->winc().value() == 56);
    REQUIRE(go_params->btime().value() == 12);
    REQUIRE(go_params->binc().value() == 54);
    REQUIRE(go_params->depth().value() == 100);
    REQUIRE(go_params->nodes().value() == 9991234567899999);
}

TEST_CASE("Go Line Test SearchMoves", "[UCIService]") {
    std::string line = "searchmoves e2e4 d7d5";
    std::stringstream line_stream{line};
    auto go_params = UCIService::parse_go_line(line_stream);
    REQUIRE(go_params);
    REQUIRE(go_params->searchmoves());
    REQUIRE_FALSE((go_params->wtime() || go_params->winc() || go_params->btime() ||
                   go_params->binc() || go_params->depth() || go_params->nodes() ||
                   go_params->infinite() || go_params->ponder() || go_params->movetime()));
    const std::vector<std::string>& move_list = go_params->searchmoves().value();
    REQUIRE(move_list[0] == "e2e4");
    REQUIRE(move_list[1] == "d7d5");
}
