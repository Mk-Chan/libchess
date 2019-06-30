#ifndef LIBCHESS_UCI_H
#define LIBCHESS_UCI_H

#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <variant>

#include "UCIService/UCIOption.h"

namespace libchess {

class UCIPositionParameters {
  public:
    UCIPositionParameters(std::string fen, std::vector<std::string> move_list) noexcept
        : fen_(std::move(fen)), move_list_(std::move(move_list)) {}

    const std::string &fen() const noexcept { return fen_; }
    const std::optional<std::vector<std::string>> &move_list() const noexcept { return move_list_; }

  private:
    std::string fen_;
    std::optional<std::vector<std::string>> move_list_;
};

class UCIGoParameters {
  public:
    UCIGoParameters(const std::optional<uint64_t> &nodes, const std::optional<int> &movetime,
                    const std::optional<int> &depth, const std::optional<int> &wtime,
                    const std::optional<int> &winc, const std::optional<int> &btime,
                    const std::optional<int> &binc, bool infinite, bool ponder,
                    std::optional<std::vector<std::string>> searchmoves)
        : nodes_(nodes), movetime_(movetime), depth_(depth), wtime_(wtime), winc_(winc),
          btime_(btime), binc_(binc), infinite_(infinite), ponder_(ponder),
          searchmoves_(std::move(searchmoves)) {}

    const std::optional<uint64_t> &nodes() const noexcept { return nodes_; }
    const std::optional<int> &movetime() const noexcept { return movetime_; }
    const std::optional<int> &depth() const noexcept { return depth_; }
    const std::optional<int> &wtime() const noexcept { return wtime_; }
    const std::optional<int> &winc() const noexcept { return winc_; }
    const std::optional<int> &btime() const noexcept { return btime_; }
    const std::optional<int> &binc() const noexcept { return binc_; }
    bool infinite() const noexcept { return infinite_; }
    bool ponder() const noexcept { return ponder_; }
    const std::optional<std::vector<std::string>> &searchmoves() const noexcept {
        return searchmoves_;
    }

  private:
    std::optional<std::uint64_t> nodes_;
    std::optional<int> movetime_;
    std::optional<int> depth_;
    std::optional<int> wtime_;
    std::optional<int> winc_;
    std::optional<int> btime_;
    std::optional<int> binc_;
    bool infinite_;
    bool ponder_;
    std::optional<std::vector<std::string>> searchmoves_;
};

class UCIService {
  private:
    using UCIOptionVariant = std::variant<UCISpinOption, UCIComboOption, UCIStringOption,
                                          UCICheckOption, UCIButtonOption>;

  public:
    void register_option(const UCIOptionVariant &uci_option) noexcept {
        if (std::holds_alternative<UCISpinOption>(uci_option)) {
            const auto& option = std::get<UCISpinOption>(uci_option);
            spin_options_[option.name()] = std::get<UCISpinOption>(uci_option);
        } else if (std::holds_alternative<UCIComboOption>(uci_option)) {
            const auto& option = std::get<UCIComboOption>(uci_option);
            combo_options_[option.name()] = std::get<UCIComboOption>(uci_option);
        } else if (std::holds_alternative<UCIStringOption>(uci_option)) {
            const auto& option = std::get<UCIStringOption>(uci_option);
            string_options_[option.name()] = std::get<UCIStringOption>(uci_option);
        } else if (std::holds_alternative<UCICheckOption>(uci_option)) {
            const auto& option = std::get<UCICheckOption>(uci_option);
            check_options_[option.name()] = std::get<UCICheckOption>(uci_option);
        } else if (std::holds_alternative<UCIButtonOption>(uci_option)) {
            const auto& option = std::get<UCIButtonOption>(uci_option);
            button_options_[option.name()] = std::get<UCIButtonOption>(uci_option);
        }
    }

    void register_position_handler(std::function<void(UCIPositionParameters)> &handler) noexcept {
        position_handler_ = handler;
    }
    void register_go_handler(std::function<void(UCIGoParameters)> &handler) noexcept {
        go_handler_ = handler;
    }
    void register_stop_handler(std::function<void(void)> &handler) noexcept {
        stop_handler_ = handler;
    }

    void run() {
        if (!(position_handler_ && go_handler_ && stop_handler_)) {
            throw std::invalid_argument{"Must register a position, go and stop handler!"};
        }

        for (auto& [name, option] : spin_options_) {
            std::cout << "option name " << name << " type spin default " << option.value()
                      << " min " << option.min_value() << " max " << option.max_value() << "\n";
        }
        for (auto& [name, option] : combo_options_) {
            std::cout << "option name " << name << " type combo default " << option.value();
            for (const auto& candidate : option.allowed_values()) {
                std::cout << " var " << candidate;
            }
            std::cout << "\n";
        }
        for (auto& [name, option] : string_options_) {
            std::cout << "option name " << name << " type string default " << option.value()
                      << "\n";
        }
        for (auto& [name, option] : check_options_) {
            std::cout << "option name " << name << " type check default " << option.value() << "\n";
        }
        for (auto& [name, option] : button_options_) {
            std::cout << "option name " << name << " type button\n";
        }

        std::string word;
        std::string line;
        while (true) {
            std::getline(std::cin, line);
            std::stringstream line_stream{line};
            line_stream >> word;
            if (word == "position") {
                auto position_parameters = parse_position_line(line_stream);
                if (position_parameters) {
                    position_handler_(*position_parameters);
                }
            } else if (word == "go") {
                auto go_parameters = parse_go_line(line_stream);
                if (go_parameters) {
                    go_handler_(*go_parameters);
                }
            } else if (word == "setoption") {
                parse_and_run_setoption_line(line_stream);
            } else if (word == "stop") {
                stop_handler_();
            } else if (word == "isready") {
                std::cout << "readyok\n";
            } else if (word == "quit" || word == "exit") {
                break;
            }
        }
    }

    void parse_and_run_setoption_line(std::stringstream &line_stream) noexcept {
        std::string tmp;
        line_stream >> tmp;
        if (tmp != "name") {
            return;
        }
        std::string name;
        line_stream >> name;
        if (button_options_.find(name) != button_options_.end()) {
            button_options_[name].handler();
            return;
        }

        line_stream >> tmp;
        if (tmp != "value") {
            return;
        }

        if (spin_options_.find(name) != spin_options_.end()) {
            int value = 0;
            if (line_stream >> value) {
                spin_options_[name].set_option(value);
            }
        } else if (combo_options_.find(name) != combo_options_.end()) {
            std::string value;
            if (line_stream >> std::quoted(value)) {
                combo_options_[name].set_option(value);
            }
        } else if (string_options_.find(name) != string_options_.end()) {
            std::string value;
            if (line_stream >> std::quoted(value)) {
                string_options_[name].set_option(value);
            }
        } else if (check_options_.find(name) != check_options_.end()) {
            bool value = false;
            if (line_stream >> value) {
                check_options_[name].set_option(value);
            }
        }
    }

    static std::optional<UCIPositionParameters>
    parse_position_line(std::stringstream &line_stream) noexcept {
        std::string fen;
        std::string tmp;
        line_stream >> tmp;
        if (tmp == "startpos") {
            fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        } else if (tmp != "fen") {
            return {};
        } else {
            line_stream >> fen;
            for (int i = 0; i < 5; ++i) {
                line_stream >> tmp;
                fen += " " + tmp;
            }
        }

        if (!line_stream) {
            return UCIPositionParameters{fen, {}};
        }
        line_stream >> tmp;
        if (tmp != "moves") {
            return UCIPositionParameters{fen, {}};
        }

        std::vector<std::string> moves;
        while (line_stream >> tmp) {
            moves.push_back(tmp);
        }

        return UCIPositionParameters{fen, moves};
    }
    static std::optional<UCIGoParameters> parse_go_line(std::stringstream &line_stream) noexcept {
        std::optional<std::uint64_t> nodes_opt;
        std::optional<int> movetime_opt;
        std::optional<int> depth_opt;
        std::optional<int> wtime_opt;
        std::optional<int> winc_opt;
        std::optional<int> btime_opt;
        std::optional<int> binc_opt;
        bool infinite = false;
        bool ponder = false;
        std::optional<std::vector<std::string>> searchmoves_opt;

        std::vector<std::string> searchmoves;
        bool filling_searchmoves = false;
        std::string tmp;
        while (line_stream >> tmp) {
            if (tmp == "nodes") {
                std::uint64_t nodes = 0;
                if (line_stream >> nodes) {
                    nodes_opt = nodes;
                }
            } else if (tmp == "movetime") {
                int movetime = 0;
                if (line_stream >> movetime) {
                    movetime_opt = movetime;
                }
            } else if (tmp == "depth") {
                int depth = 0;
                if (line_stream >> depth) {
                    depth_opt = depth;
                }
            } else if (tmp == "wtime") {
                int wtime = 0;
                if (line_stream >> wtime) {
                    wtime_opt = wtime;
                }
            } else if (tmp == "winc") {
                int winc = 0;
                if (line_stream >> winc) {
                    winc_opt = winc;
                }
            } else if (tmp == "btime") {
                int btime = 0;
                if (line_stream >> btime) {
                    btime_opt = btime;
                }
            } else if (tmp == "binc") {
                int binc = 0;
                if (line_stream >> binc) {
                    binc_opt = binc;
                }
            } else if (tmp == "infinite") {
                infinite = true;
            } else if (tmp == "ponder") {
                ponder = true;
            } else if (tmp == "searchmoves") {
                filling_searchmoves = true;
                continue;
            } else if (filling_searchmoves) {
                searchmoves.push_back(tmp);
                continue;
            } else {
                break;
            }
            filling_searchmoves = false;
        }
        if (!searchmoves.empty()) {
            searchmoves_opt = searchmoves;
        }

        return UCIGoParameters{nodes_opt, movetime_opt, depth_opt, wtime_opt, winc_opt,
                               btime_opt, binc_opt,     infinite,  ponder,    searchmoves_opt};
    }

  private:
    std::unordered_map<std::string, UCISpinOption> spin_options_;
    std::unordered_map<std::string, UCIComboOption> combo_options_;
    std::unordered_map<std::string, UCIStringOption> string_options_;
    std::unordered_map<std::string, UCICheckOption> check_options_;
    std::unordered_map<std::string, UCIButtonOption> button_options_;

    std::function<void(UCIPositionParameters)> position_handler_;
    std::function<void(UCIGoParameters)> go_handler_;
    std::function<void(void)> stop_handler_;
};

} // namespace libchess

#endif // LIBCHESS_UCI_H
