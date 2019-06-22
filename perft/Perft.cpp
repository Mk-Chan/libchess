#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <string_view>

#include "../Position.h"

using namespace libchess;
using namespace constants;

inline long long int perft(Position& pos, int depth) {
    long long int count = 0LL;
    MoveList move_list = pos.legal_move_list();
    if (depth == 1) {
        return move_list.size();
    }
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
    int line_nr = 0;
    bool failed = false;
    while (std::getline(file, line)) {
        line_nr++;
        std::string_view line_view{line};
        auto delim_pos = line_view.find_first_of(';');
        if (delim_pos == std::string_view::npos) {
            break;
        }
        auto fen = line_view.substr(0, delim_pos);
        Position pos = *Position::from_fen(fen.data());
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
            auto start_ts = std::chrono::system_clock::now();
            auto actual_result = perft(pos, depth);
            auto end_ts = std::chrono::system_clock::now();
            std::chrono::duration<double> diff_ts = end_ts - start_ts;
            if (actual_result != expected_result) {
                std::cout << "FAILED EPD: " << line << " (" << line_nr << ")\n";
                std::cout << "EXPECTED: " << expected_result << ", GOT: " << actual_result << "\n";
                failed = true;
            } else {
                double time_s = diff_ts.count();
                double nps = time_s > 0.0 ? actual_result / time_s : actual_result;
                std::cout << "line: " << line_nr << ", depth: " << depth
                          << ", nps: " << std::setprecision(4) << nps
                          << ", count: " << actual_result << "\n";
            }
        }
    }
    if (failed) {
        std::cout << "\nPerft suite failed!\n";
        return 1;
    }
    std::cout << "\nPerft suite passed!\n";
    return 0;
}
