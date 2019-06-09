#include <fstream>
#include <string>
#include <string_view>

#include "../Position.h"

using namespace libchess;
using namespace constants;

constexpr inline long long int perft(Position& pos, int depth) {
    if (depth <= 0) {
        return 1LL;
    }
    long long int count = 0LL;
    MoveList move_list = pos.legal_move_list();
    for (Move move : move_list) {
        pos.make_move(move);
        count += perft(pos, depth - 1);
        pos.unmake_move();
    }
    return count;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: ./libchess_perft <file-path> <max-depth>\n";
        return 1;
    }
    std::string epd_path = argv[1];
    int max_depth = std::atoi(argv[2]);
    std::ifstream file{epd_path};
    std::string line;
    while (std::getline(file, line)) {
        std::string_view line_view{line};
        auto delim_pos = line_view.find_first_of(';');
        if (delim_pos == std::string_view::npos) {
            break;
        }
        auto fen = line_view.substr(0, delim_pos);
        Position pos{fen.data()};
        while (true) {
            auto start_pos = delim_pos + 2;
            delim_pos = line_view.find_first_of(';', start_pos);
            if (delim_pos == std::string_view::npos) {
                break;
            }
            auto sep_pos = line_view.find_first_of(' ', start_pos);
            auto depth_token = line_view.substr(start_pos + 1, sep_pos - start_pos);
            auto result_token = line_view.substr(sep_pos + 1, delim_pos - sep_pos - 1);
            char* endptr;
            auto depth = std::strtoll(depth_token.begin(), &endptr, 10);
            if (depth > max_depth) {
                break;
            }
            auto expected_result = std::strtoll(result_token.begin(), &endptr, 10);
            auto actual_result = perft(pos, depth);
            if (actual_result != expected_result) {
                std::cout << "FAILED EPD: " << line << "\n";
                std::cout << "EXPECTED: " << expected_result << ", GOT: " << actual_result << "\n";
            }
        }
    }
    return 0;
}
