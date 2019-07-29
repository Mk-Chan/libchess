#ifndef LIBCHESS_TUNER_H
#define LIBCHESS_TUNER_H

#include <iostream>
#include <string>
#include <vector>

namespace libchess {

class TunableParameter {
  public:
    TunableParameter(std::string name, int value) noexcept
        : name_(std::move(name)), value_(value) {}

    TunableParameter operator+(int rhs) const noexcept {
        return TunableParameter{name(), value() + rhs};
    }
    TunableParameter operator-(int rhs) const noexcept {
        return TunableParameter{name(), value_ - rhs};
    }
    void operator+=(int rhs) noexcept { value_ += rhs; }
    void operator-=(int rhs) noexcept { value_ -= rhs; }

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] int value() const noexcept { return value_; }

    void set_value(int value) noexcept { value_ = value; }

    [[nodiscard]] std::string to_str() const noexcept {
        return name_ + ": " + std::to_string(value_);
    }

  private:
    std::string name_;
    int value_;
};

enum class Result { BLACK_WIN, DRAW, WHITE_WIN };

template <class Position> class NormalizedResult {
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

    [[nodiscard]] Position& position() noexcept { return position_; }
    [[nodiscard]] double value() const noexcept { return value_; }

  private:
    Position position_;
    double value_;
};

template <class Position> class Tuner {
  public:
    Tuner(std::vector<NormalizedResult<Position>> normalized_results,
          std::vector<TunableParameter> tunable_parameters,
          std::function<int(Position&, const std::vector<TunableParameter>&)> eval_function)
        : normalized_results_(std::move(normalized_results)),
          tunable_parameters_(std::move(tunable_parameters)),
          eval_function_(std::move(eval_function)) {}

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
        for (int i = 0; i < tunable_parameters_.size(); ++i) {
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

            for (int i = 0; i < tunable_parameters_.size(); ++i) {
                TunableParameter& parameter = tunable_parameters_[i];
                LocalParameterTuningData& tuning_data = parameter_tuning_data[i];
                std::cout << parameter.name() << ": " << parameter.value() << " improving "
                          << tuning_data.improving() << "\n";
            }
            std::cout << "--\n";

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

    void tune() noexcept { local_tune(); }

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
    struct LocalParameterTuningData {
      public:
        [[nodiscard]] bool improving() const noexcept { return direction_ != 0; }
        [[nodiscard]] bool done() const noexcept { return done_; }
        [[nodiscard]] int direction() const noexcept { return direction_; }
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
        void reverse_direction() noexcept { direction_ = -direction_; }
        void set_done(bool value) noexcept { done_ = value; }
        void set_direction(int value) noexcept { direction_ = value; }

      private:
        constexpr static std::array<int, 7> increment_values{100, 50, 25, 12, 6, 3, 1};

        bool done_ = false;
        int increment_offset_ = 0;
        int direction_ = 1;
    };
    [[nodiscard]] static bool
    all_done(const std::vector<LocalParameterTuningData>& tuning_data_list) noexcept {
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

} // namespace libchess

#endif // LIBCHESS_TUNER_H
