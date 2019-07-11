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

    [[nodiscard]] const std::string& name() const { return name_; }
    [[nodiscard]] int value() const { return value_; }

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
    NormalizedResult(const Position& position, Result result) noexcept
        : position_(std::move(position)) {
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

    [[nodiscard]] const Position& position() const noexcept { return position_; }
    [[nodiscard]] float value() const noexcept { return value_; }

  private:
    Position position_;
    float value_;
};

template <class Position> class Tuner {
  public:
    Tuner(std::vector<NormalizedResult<Position>> normalized_results,
          std::vector<TunableParameter> tunable_parameters,
          std::function<int(const Position&, const std::vector<TunableParameter>&)> eval_function)
        : normalized_results_(std::move(normalized_results)),
          tunable_parameters_(std::move(tunable_parameters)),
          eval_function_(std::move(eval_function)) {}

    [[nodiscard]] const std::vector<TunableParameter>& tunable_parameters() const noexcept {
        return tunable_parameters_;
    }

    [[nodiscard]] float error() const noexcept {
        float sum =
            std::accumulate(normalized_results_.cbegin(), normalized_results_.cend(), 0.0,
                            [this](int, const NormalizedResult<Position>& normalized_result) {
                                float normalized_eval = sigmoid(eval(normalized_result.position()));
                                float err = normalized_result.value() - normalized_eval;
                                return err * err;
                            });
        return sum / float(normalized_results_.size());
    }

    void step() noexcept {
        float least_error = error();
        int increment = 1;
        for (TunableParameter& tunable_parameter : tunable_parameters_) {
            int start_value = tunable_parameter.value();
            tunable_parameter.set_value(start_value + increment);
            float new_error = error();
            if (new_error < least_error) {
                least_error = new_error;
            } else {
                tunable_parameter.set_value(start_value - increment);
                new_error = error();
                if (new_error < least_error) {
                    least_error = new_error;
                } else {
                    tunable_parameter.set_value(start_value);
                }
            }
        }
    }

    void tune() noexcept {
        while (true) {
            float start_error = error();
            step();
            float end_error = error();
            if (end_error >= start_error) {
                break;
            }
        }
    }

    void display() const noexcept {
        for (auto& param : tunable_parameters_) {
            std::cout << param.to_str() << "\n";
        }
    }

  protected:
    static float sigmoid(int score, float k = 1.13) noexcept {
        return 1.0 / (1.0 + std::pow(10.0, -k * score / 400.0));
    }

    int eval(const Position& position) const noexcept {
        return eval_function_(position, tunable_parameters_);
    }

  private:
    std::vector<NormalizedResult<Position>> normalized_results_{};
    std::vector<TunableParameter> tunable_parameters_{};
    std::function<int(const Position&, const std::vector<TunableParameter>&)> eval_function_{};
};

} // namespace libchess

#endif // LIBCHESS_TUNER_H
