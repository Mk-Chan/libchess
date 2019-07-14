#ifndef LIBCHESS_UCISERVICE_H
#define LIBCHESS_UCISERVICE_H

#include "UCIOption.h"

#include <any>
#include <atomic>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

namespace libchess {

class UCIScore {
  public:
    enum class ScoreType { CENTIPAWNS, MATE };

    UCIScore(int value, ScoreType score_type) noexcept : value_(value), score_type_(score_type) {}

    [[nodiscard]] int value() const noexcept { return value_; }
    [[nodiscard]] ScoreType score_type() const noexcept { return score_type_; }

  private:
    int value_;
    ScoreType score_type_;
};

class UCIMoveList {
  public:
    explicit UCIMoveList(std::vector<std::string> move_list) noexcept
        : move_list_(std::move(move_list)) {}

    [[nodiscard]] const std::string& operator[](int i) const noexcept { return move_list_[i]; }

    [[nodiscard]] const std::vector<std::string>& move_list() const { return move_list_; }
    [[nodiscard]] bool empty() const { return move_list_.empty(); }

    [[nodiscard]] std::string to_str() const noexcept {
        std::string pv_line = move_list_[0];
        for (auto move_iter = move_list_.begin() + 1; move_iter != move_list_.end(); ++move_iter) {
            pv_line += " " + *move_iter;
        }
        return pv_line;
    }

  private:
    std::vector<std::string> move_list_;
};

class UCIPositionParameters {
  public:
    UCIPositionParameters(std::string fen, std::optional<UCIMoveList> move_list) noexcept
        : fen_(std::move(fen)), move_list_(std::move(move_list)) {}

    [[nodiscard]] const std::string& fen() const noexcept { return fen_; }
    [[nodiscard]] const std::optional<UCIMoveList>& move_list() const noexcept {
        return move_list_;
    }

  private:
    std::string fen_;
    std::optional<UCIMoveList> move_list_;
};

class UCIGoParameters {
  public:
    UCIGoParameters(const std::optional<uint64_t>& nodes, const std::optional<int>& movetime,
                    const std::optional<int>& depth, const std::optional<int>& wtime,
                    const std::optional<int>& winc, const std::optional<int>& btime,
                    const std::optional<int>& binc, bool infinite, bool ponder,
                    std::optional<std::vector<std::string>> searchmoves)
        : nodes_(nodes), movetime_(movetime), depth_(depth), wtime_(wtime), winc_(winc),
          btime_(btime), binc_(binc), infinite_(infinite), ponder_(ponder),
          searchmoves_(std::move(searchmoves)) {}

    [[nodiscard]] const std::optional<uint64_t>& nodes() const noexcept { return nodes_; }
    [[nodiscard]] const std::optional<int>& movetime() const noexcept { return movetime_; }
    [[nodiscard]] const std::optional<int>& depth() const noexcept { return depth_; }
    [[nodiscard]] const std::optional<int>& wtime() const noexcept { return wtime_; }
    [[nodiscard]] const std::optional<int>& winc() const noexcept { return winc_; }
    [[nodiscard]] const std::optional<int>& btime() const noexcept { return btime_; }
    [[nodiscard]] const std::optional<int>& binc() const noexcept { return binc_; }
    [[nodiscard]] bool infinite() const noexcept { return infinite_; }
    [[nodiscard]] bool ponder() const noexcept { return ponder_; }
    [[nodiscard]] const std::optional<UCIMoveList>& searchmoves() const noexcept {
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
    std::optional<UCIMoveList> searchmoves_;
};

class UCIInfoParameters {
  public:
    UCIInfoParameters() = default;
    explicit UCIInfoParameters(const std::unordered_map<std::string, std::any>& values) noexcept {
        auto key_present = [values](const std::string& key) {
            return values.find(key) != values.end();
        };
        if (key_present("depth")) {
            depth_ = std::any_cast<int>(values.at("depth"));
        }
        if (key_present("seldepth")) {
            seldepth_ = std::any_cast<int>(values.at("seldepth"));
        }
        if (key_present("time")) {
            time_ = std::any_cast<int>(values.at("time"));
        }
        if (key_present("nodes")) {
            nodes_ = std::any_cast<std::uint64_t>(values.at("nodes"));
        }
        if (key_present("pv")) {
            pv_ = std::any_cast<UCIMoveList>(values.at("pv"));
        }
        if (key_present("multipv")) {
            multipv_ = std::any_cast<std::vector<UCIMoveList>>(values.at("multipv"));
        }
        if (key_present("score")) {
            score_ = std::any_cast<UCIScore>(values.at("score"));
        }
        if (key_present("currmove")) {
            currmove_ = std::any_cast<std::string>(values.at("currmove"));
        }
        if (key_present("currmovenumber")) {
            currmovenumber_ = std::any_cast<int>(values.at("currmovenumber"));
        }
        if (key_present("hashfull")) {
            hashfull_ = std::any_cast<int>(values.at("hashfull"));
        }
        if (key_present("nps")) {
            nps_ = std::any_cast<int>(values.at("nps"));
        }
        if (key_present("tbhits")) {
            tbhits_ = std::any_cast<int>(values.at("tbhits"));
        }
        if (key_present("cpuload")) {
            cpuload_ = std::any_cast<int>(values.at("cpuload"));
        }
        if (key_present("string")) {
            string_ = std::any_cast<std::string>(values.at("string"));
        }
        if (key_present("refutation")) {
            refutation_ = std::any_cast<UCIMoveList>(values.at("refutation"));
        }
        if (key_present("currline")) {
            currline_ = std::any_cast<std::vector<UCIMoveList>>(values.at("currline"));
        }
    }

    [[nodiscard]] const std::optional<int>& depth() const noexcept { return depth_; }
    [[nodiscard]] const std::optional<int>& seldepth() const noexcept { return seldepth_; }
    [[nodiscard]] const std::optional<int>& time() const noexcept { return time_; }
    [[nodiscard]] const std::optional<uint64_t>& nodes() const noexcept { return nodes_; }
    [[nodiscard]] const std::optional<UCIMoveList>& pv() const noexcept { return pv_; }
    [[nodiscard]] const std::optional<std::vector<UCIMoveList>>& multipv() const noexcept {
        return multipv_;
    }
    [[nodiscard]] const std::optional<UCIScore>& score() const noexcept { return score_; }
    [[nodiscard]] const std::optional<std::string>& currmove() const noexcept { return currmove_; }
    [[nodiscard]] const std::optional<int>& currmovenumber() const noexcept {
        return currmovenumber_;
    }
    [[nodiscard]] const std::optional<int>& hashfull() const noexcept { return hashfull_; }
    [[nodiscard]] const std::optional<int>& nps() const noexcept { return nps_; }
    [[nodiscard]] const std::optional<int>& tbhits() const noexcept { return tbhits_; }
    [[nodiscard]] const std::optional<int>& cpuload() const noexcept { return cpuload_; }
    [[nodiscard]] const std::optional<UCIMoveList>& refutation() const noexcept {
        return refutation_;
    }
    [[nodiscard]] const std::optional<std::vector<UCIMoveList>>& currline() const noexcept {
        return currline_;
    }
    [[nodiscard]] const std::optional<std::string>& string() const noexcept { return string_; }
    [[nodiscard]] bool empty() const noexcept {
        return !(depth_ || seldepth_ || time_ || nodes_ || pv_ || multipv_ || score_ || currmove_ ||
                 currmovenumber_ || hashfull_ || nps_ || tbhits_ || cpuload_ || refutation_ ||
                 currline_ || string_);
    }

    void set_depth(const std::optional<int> depth) noexcept { depth_ = depth; }
    void set_seldepth(const std::optional<int> seldepth) noexcept { seldepth_ = seldepth; }
    void set_time(const std::optional<int> time) noexcept { time_ = time; }
    void set_nodes(const std::optional<uint64_t> nodes) noexcept { nodes_ = nodes; }
    void set_pv(const std::optional<UCIMoveList>& pv) noexcept { pv_ = pv; }
    void set_multipv(const std::optional<std::vector<UCIMoveList>>& multipv) noexcept {
        multipv_ = multipv;
    }
    void set_score(const std::optional<UCIScore> score) noexcept { score_ = score; }
    void set_currmove(const std::optional<std::string>& currmove) noexcept { currmove_ = currmove; }
    void set_currmovenumber(const std::optional<int> currmovenumber) noexcept {
        currmovenumber_ = currmovenumber;
    }
    void set_hashfull(const std::optional<int> hashfull) noexcept { hashfull_ = hashfull; }
    void set_nps(const std::optional<int> nps) noexcept { nps_ = nps; }
    void set_tbhits(const std::optional<int> tbhits) noexcept { tbhits_ = tbhits; }
    void set_cpuload(const std::optional<int> cpuload) noexcept { cpuload_ = cpuload; }
    void set_refutation(const std::optional<UCIMoveList>& refutation) noexcept {
        refutation_ = refutation;
    }
    void set_currline(const std::optional<std::vector<UCIMoveList>>& currline) noexcept {
        currline_ = currline;
    }
    void set_string(const std::optional<std::string>& string) noexcept { string_ = string; }

  private:
    std::optional<int> depth_;
    std::optional<int> seldepth_;
    std::optional<int> time_;
    std::optional<std::uint64_t> nodes_;
    std::optional<UCIMoveList> pv_;
    std::optional<std::vector<UCIMoveList>> multipv_;
    std::optional<UCIScore> score_;
    std::optional<std::string> currmove_;
    std::optional<int> currmovenumber_;
    std::optional<int> hashfull_;
    std::optional<int> nps_;
    std::optional<int> tbhits_;
    std::optional<int> cpuload_;
    std::optional<UCIMoveList> refutation_;
    std::optional<std::vector<UCIMoveList>> currline_;
    std::optional<std::string> string_;
};

class UCIService {
  public:
    void register_option(const UCISpinOption& uci_option) noexcept {
        spin_options_[uci_option.name()] = uci_option;
    }
    void register_option(const UCIComboOption& uci_option) noexcept {
        combo_options_[uci_option.name()] = uci_option;
    }
    void register_option(const UCIStringOption& uci_option) noexcept {
        string_options_[uci_option.name()] = uci_option;
    }
    void register_option(const UCICheckOption& uci_option) noexcept {
        check_options_[uci_option.name()] = uci_option;
    }
    void register_option(const UCIButtonOption& uci_option) noexcept {
        button_options_[uci_option.name()] = uci_option;
    }

    void
    register_position_handler(std::function<void(const UCIPositionParameters&)> handler) noexcept {
        position_handler_ = std::move(handler);
    }
    void register_go_handler(std::function<void(const UCIGoParameters&)> handler) noexcept {
        go_handler_ = std::move(handler);
    }
    void register_stop_handler(std::function<void(void)> handler) noexcept {
        stop_handler_ = std::move(handler);
    }
    void register_handler(const std::string& command,
                          std::function<void(std::istringstream&)> handler) noexcept {
        command_handlers_[command] = std::move(handler);
    }

    void stop() { keep_running_ = false; }
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
        std::cout << "uciok\n";

        std::string word;
        std::string line;
        std::optional<std::thread> go_thread;
        keep_running_ = true;
        while (keep_running_) {
            std::getline(std::cin, line);
            std::istringstream line_stream{line};
            line_stream >> word;
            if (command_handlers_.find(word) != command_handlers_.end()) {
                command_handlers_[word](line_stream);
            } else if (word == "position") {
                auto position_parameters = parse_position_line(line_stream);
                if (position_parameters) {
                    position_handler_(*position_parameters);
                }
            } else if (word == "go") {
                auto go_parameters = parse_go_line(line_stream);
                if (go_parameters) {
                    go_thread = std::thread{go_handler_, *go_parameters};
                }
            } else if (word == "stop") {
                if (go_thread) {
                    stop_handler_();
                    go_thread->join();
                    go_thread = {};
                }
            } else if (word == "setoption") {
                parse_and_run_setoption_line(line_stream);
            } else if (word == "isready") {
                std::cout << "readyok\n";
            } else if (word == "quit" || word == "exit") {
                break;
            }
        }
    }

    void parse_and_run_setoption_line(std::istringstream& line_stream) noexcept {
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

    static void bestmove(const std::string& move,
                         const std::optional<std::string>& ponder_move = {}) noexcept {
        std::cout << "bestmove " << move;
        if (ponder_move) {
            std::cout << " ponder " << *ponder_move;
        }
        std::cout << "\n";
    }
    static void info(const UCIInfoParameters& info_parameters) noexcept {
        if (info_parameters.empty()) {
            return;
        }

        std::cout << "info";
        if (info_parameters.score()) {
            auto score = *info_parameters.score();
            if (score.score_type() == UCIScore::ScoreType::CENTIPAWNS) {
                std::cout << " cp " << score.value();
            } else if (score.score_type() == UCIScore::ScoreType::MATE) {
                std::cout << " mate " << score.value();
            }
        }
        if (info_parameters.depth()) {
            std::cout << " depth " << *info_parameters.depth();
        }
        if (info_parameters.seldepth()) {
            std::cout << " seldepth " << *info_parameters.seldepth();
        }
        if (info_parameters.time()) {
            std::cout << " time " << *info_parameters.time();
        }
        if (info_parameters.nodes()) {
            std::cout << " nodes " << *info_parameters.nodes();
        }
        if (info_parameters.currmove()) {
            std::cout << " currmove " << *info_parameters.currmove();
        }
        if (info_parameters.currmovenumber()) {
            std::cout << " currmovenumber " << *info_parameters.currmovenumber();
        }
        if (info_parameters.hashfull()) {
            std::cout << " hashfull " << *info_parameters.hashfull();
        }
        if (info_parameters.nps()) {
            std::cout << " nps " << *info_parameters.nps();
        }
        if (info_parameters.tbhits()) {
            std::cout << " tbhits " << *info_parameters.tbhits();
        }
        if (info_parameters.cpuload()) {
            std::cout << " cpuload " << *info_parameters.cpuload();
        }
        if (info_parameters.pv()) {
            auto pv = *info_parameters.pv();
            if (!pv.empty()) {
                std::cout << " pv " << pv.to_str();
            }
        }
        if (info_parameters.refutation()) {
            auto refutation = *info_parameters.refutation();
            if (!refutation.empty()) {
                std::cout << " refutation " << refutation.to_str();
            }
        }
        if (info_parameters.string()) {
            std::cout << " string " << *info_parameters.string();
        }
        std::cout << "\n";
        if (info_parameters.multipv()) {
            auto multipv = *info_parameters.multipv();
            for (unsigned long i = 0; i < multipv.size(); ++i) {
                if (multipv.empty()) {
                    continue;
                }
                std::cout << "info multipv " << (i + 1) << " " << multipv[i].to_str() << "\n";
            }
        }
        if (info_parameters.currline()) {
            auto currline = *info_parameters.currline();
            for (unsigned long i = 0; i < currline.size(); ++i) {
                if (currline.empty()) {
                    continue;
                }
                std::cout << "info currline " << (i + 1) << " " << currline[i].to_str() << "\n";
            }
        }
    }

    static std::optional<UCIPositionParameters>
    parse_position_line(std::istringstream& line_stream) noexcept {
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

        return UCIPositionParameters{fen, UCIMoveList{moves}};
    }
    static std::optional<UCIGoParameters> parse_go_line(std::istringstream& line_stream) noexcept {
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
    std::unordered_map<std::string, std::function<void(std::istringstream&)>> command_handlers_;

    std::atomic<bool> keep_running_{true};
};

} // namespace libchess

#endif // LIBCHESS_UCISERVICE_H
