#ifndef LIBCHESS_TUNER_H
#define LIBCHESS_TUNER_H

#include <array>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace libchess {

class TunableParameter {
   public:
    TunableParameter(std::string name, int value) noexcept : name_(std::move(name)), value_(value) {
    }

    TunableParameter operator+(int rhs) const noexcept {
        return TunableParameter{name(), value() + rhs};
    }
    TunableParameter operator-(int rhs) const noexcept {
        return TunableParameter{name(), value_ - rhs};
    }
    void operator+=(int rhs) noexcept {
        value_ += rhs;
    }
    void operator-=(int rhs) noexcept {
        value_ -= rhs;
    }

    [[nodiscard]] const std::string& name() const noexcept {
        return name_;
    }
    [[nodiscard]] int value() const noexcept {
        return value_;
    }

    void set_value(int value) noexcept {
        value_ = value;
    }

    [[nodiscard]] std::string to_str() const noexcept {
        return name_ + ": " + std::to_string(value_);
    }

   private:
    std::string name_;
    int value_;
};

enum class Result
{
    BLACK_WIN,
    DRAW,
    WHITE_WIN
};

template <class Position>
class NormalizedResult {
   public:
    NormalizedResult(Position position, Result result) noexcept : position_(std::move(position)) {
        switch (result) {
            case Result::BLACK_WIN:
                value_ = 0.0;
                break;
            case Result::DRAW:
                value_ = 0.5;
                break;
            case Result::WHITE_WIN:
                value_ = 1.0;
                break;
        }
    }

    [[nodiscard]] Position& position() noexcept {
        return position_;
    }
    [[nodiscard]] double value() const noexcept {
        return value_;
    }

    static std::vector<NormalizedResult<Position>> parse_epd(
        const std::string& path,
        std::function<Position(const std::string&)> fen_parser,
        const std::string& result_opcode = "c9") noexcept {
        std::string line;
        std::ifstream file_stream{path};
        std::vector<NormalizedResult<Position>> normalized_results;
        while (true) {
            std::getline(file_stream, line);
            if (line.empty()) {
                break;
            }
            std::string_view line_view{line};
            auto curr_pos = line_view.begin();
            for (unsigned i = 0; i < 4; ++i) {
                curr_pos = std::find(curr_pos + 1, line_view.end(), ' ');
            }
            std::string fen{line_view.begin(), curr_pos};
            std::string post_fen{curr_pos + 1, line_view.end()};
            Result result = [&result_opcode, &post_fen] {
                std::istringstream post_fen_stream{post_fen};
                std::string opcode;
                while (post_fen_stream >> opcode) {
                    if (opcode == ";") {
                        break;
                    }
                    std::string value;
                    post_fen_stream >> std::quoted(value);
                    if (opcode != result_opcode) {
                        continue;
                    }
                    if (value == "1-0") {
                        return Result::WHITE_WIN;
                    } else if (value == "0-1") {
                        return Result::BLACK_WIN;
                    } else {
                        return Result::DRAW;
                    }
                }
                return Result::DRAW;
            }();

            normalized_results.push_back(NormalizedResult{fen_parser(fen), result});
        }
        return normalized_results;
    }

   private:
    Position position_;
    double value_;
};

template <class Position>
class Tuner {
   public:
    Tuner(std::vector<NormalizedResult<Position>> normalized_results,
          std::vector<TunableParameter> tunable_parameters,
          std::function<int(Position&, const std::vector<TunableParameter>&)> eval_function)
        : normalized_results_(std::move(normalized_results)),
          tunable_parameters_(std::move(tunable_parameters)),
          eval_function_(std::move(eval_function)) {
    }

    [[nodiscard]] const std::vector<TunableParameter>& tunable_parameters() const noexcept {
        return tunable_parameters_;
    }

    [[nodiscard]] double error() noexcept {
        double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
        for (unsigned i = 0; i < normalized_results_.size(); ++i) {
            auto& normalized_result = normalized_results_.at(i);
            double normalized_eval = sigmoid(eval(normalized_result.position()));
            double err = normalized_result.value() - normalized_eval;
            sum += err * err;
        }
        return sum / double(normalized_results_.size());
    }

    void local_tune() noexcept {
        double least_error = error();
        std::vector<LocalParameterTuningData> parameter_tuning_data;
        parameter_tuning_data.reserve(tunable_parameters_.size());
        for (unsigned i = 0; i < tunable_parameters_.size(); ++i) {
            parameter_tuning_data.push_back(LocalParameterTuningData{});
        }

        while (!all_done(parameter_tuning_data)) {
            auto param_iter = tunable_parameters_.begin();
            auto tune_data_iter = parameter_tuning_data.begin();
            for (; param_iter != tunable_parameters_.end() &&
                   tune_data_iter != parameter_tuning_data.end();
                 ++param_iter, ++tune_data_iter) {
                if (tune_data_iter->done()) {
                    continue;
                }

                *param_iter += tune_data_iter->increment();
                double new_error = error();
                if (new_error < least_error) {
                    least_error = new_error;
                } else {
                    tune_data_iter->reverse_direction();
                    *param_iter += 2 * tune_data_iter->increment();
                    new_error = error();
                    if (new_error < least_error) {
                        least_error = new_error;
                    } else {
                        *param_iter -= tune_data_iter->increment();
                        tune_data_iter->set_direction(0);
                    }
                }
            }

            for (unsigned i = 0; i < tunable_parameters_.size(); ++i) {
                TunableParameter& parameter = tunable_parameters_[i];
                LocalParameterTuningData& tuning_data = parameter_tuning_data[i];
                std::cout << parameter.name() << ": " << parameter.value() << " improving "
                          << tuning_data.improving() << "\n";
            }
            std::cout << "Least error: " << least_error << "\n";

            for (LocalParameterTuningData& tune_data : parameter_tuning_data) {
                if (!tune_data.improving()) {
                    if (tune_data.can_reduce_increment()) {
                        tune_data.reduce_increment();
                        tune_data.set_direction(1);
                    } else {
                        tune_data.set_done(true);
                    }
                }
            }
        }
    }

    void simulated_annealing(int max_steps) noexcept {
        std::random_device random_device;
        std::mt19937 rng{random_device()};
        std::uniform_int_distribution<> increment_distribution{0, increment_values.size() - 1};
        std::uniform_int_distribution<> parameter_distribution{0, tunable_parameters_.size() - 1};

        auto random_bool = [&](double probability) {
            std::bernoulli_distribution bool_distribution{probability};
            return bool_distribution(rng);
        };
        auto random_increment = [&]() {
            return (random_bool(0.5) ? 1 : -1) * increment_values[increment_distribution(rng)];
        };

        double current_error = error();
        for (int step = 0; step < max_steps; ++step) {
            double temperature = 1.0 / (1.667 * (1.0 + double(step)));

            int increment = random_increment();
            TunableParameter& tunable_parameter = tunable_parameters_[parameter_distribution(rng)];
            tunable_parameter += increment;

            double new_error = error();

            double acceptance_probability =
                new_error < current_error
                    ? 1.0
                    : std::exp(-(new_error - current_error) / double(temperature));
            if (random_bool(acceptance_probability)) {
                current_error = new_error;
            } else {
                tunable_parameter -= increment;
            }

            display();
            std::cout << "acceptance prob: " << acceptance_probability << " step: " << step
                      << " temperature: " << temperature << " error: " << current_error << "\n";
        }
    }

    void tune() noexcept {
        simulated_annealing(1000);
        local_tune();
    }

    void display() const noexcept {
        for (auto& param : tunable_parameters_) {
            std::cout << param.to_str() << "\n";
        }
    }

   protected:
    [[nodiscard]] static double sigmoid(int score, double k = 1.13) noexcept {
        return 1.0 / (1.0 + std::pow(10.0, -k * score / 400.0));
    }

    [[nodiscard]] int eval(Position& position) noexcept {
        return eval_function_(position, tunable_parameters_);
    }

   private:
    constexpr static std::array<int, 7> increment_values{100, 50, 25, 12, 6, 3, 1};

    struct LocalParameterTuningData {
       public:
        [[nodiscard]] bool improving() const noexcept {
            return direction_ != 0;
        }
        [[nodiscard]] bool done() const noexcept {
            return done_;
        }
        [[nodiscard]] int direction() const noexcept {
            return direction_;
        }
        [[nodiscard]] int increment() const noexcept {
            return direction_ * increment_values[increment_offset_];
        }
        [[nodiscard]] bool can_reduce_increment() const noexcept {
            return increment_offset_ < increment_values.size() - 1;
        }

        void reduce_increment() noexcept {
            if (can_reduce_increment()) {
                ++increment_offset_;
            }
        }
        void reverse_direction() noexcept {
            direction_ = -direction_;
        }
        void set_done(bool value) noexcept {
            done_ = value;
        }
        void set_direction(int value) noexcept {
            direction_ = value;
        }

       private:
        bool done_ = false;
        unsigned increment_offset_ = 0;
        int direction_ = 1;
    };

    [[nodiscard]] static bool all_done(
        const std::vector<LocalParameterTuningData>& tuning_data_list) noexcept {
        for (auto& tuning_data : tuning_data_list) {
            if (!tuning_data.done()) {
                return false;
            }
        }
        return true;
    }

    std::vector<NormalizedResult<Position>> normalized_results_{};
    std::vector<TunableParameter> tunable_parameters_{};
    std::function<int(Position&, const std::vector<TunableParameter>&)> eval_function_{};
};

}  // namespace libchess

#endif  // LIBCHESS_TUNER_H
