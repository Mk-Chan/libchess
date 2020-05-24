#ifndef LIBCHESS_UCIOPTION_H
#define LIBCHESS_UCIOPTION_H

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace libchess {

template <class T>
class UCIOption {
   public:
    using value_type = T;

    UCIOption() = default;
    UCIOption(std::string name, value_type value, std::function<void(const T&)> handler)
        : name_(std::move(name)), value_(std::move(value)), handler_(std::move(handler)) {
    }

    [[nodiscard]] std::string name() const noexcept {
        return name_;
    }
    [[nodiscard]] value_type value() const noexcept {
        return value_;
    };

    void handler(std::optional<value_type> value) const noexcept {
        return handler_(value);
    }

    virtual void set_option(const value_type& value) noexcept {
        value_ = value;
        handler_(value);
    };

   protected:
    std::string name_;
    value_type value_{};
    std::function<void(const T&)> handler_;
};

template <>
class UCIOption<void> {
   public:
    using value_type = void;

    UCIOption() = default;
    UCIOption(std::string name, std::function<void(void)> handler) noexcept
        : name_(std::move(name)), handler_(std::move(handler)) {
    }

    [[nodiscard]] const std::string& name() const noexcept {
        return name_;
    }
    void handler() const noexcept {
        handler_();
    }

   protected:
    std::string name_;
    std::function<void(void)> handler_;
};

class UCISpinOption : public UCIOption<int> {
   public:
    UCISpinOption() : min_value_(0), max_value_(0) {
    }
    UCISpinOption(const std::string& name,
                  const value_type& value,
                  const value_type& min_value,
                  const value_type& max_value,
                  const std::function<void(const value_type&)>& handler) noexcept
        : UCIOption<value_type>(name, value, handler),
          min_value_(min_value),
          max_value_(max_value) {
    }

    [[nodiscard]] value_type min_value() const noexcept {
        return min_value_;
    }
    [[nodiscard]] value_type max_value() const noexcept {
        return max_value_;
    }
    void set_option(const value_type& value) noexcept override {
        if (value >= min_value() && value <= max_value()) {
            value_ = value;
            handler_(value);
        }
    };

   private:
    value_type min_value_;
    value_type max_value_;
};

class UCIComboOption : public UCIOption<std::string> {
   public:
    UCIComboOption() : UCIOption<std::string>() {
    }
    UCIComboOption(const std::string& name,
                   const value_type& value,
                   std::unordered_set<value_type> allowed_values,
                   const std::function<void(const value_type&)>& handler) noexcept
        : UCIOption<value_type>(name, value, handler), allowed_values_(std::move(allowed_values)) {
    }

    [[nodiscard]] bool is_allowed(const value_type& value) const noexcept {
        return allowed_values_.find(value) != allowed_values_.end();
    }
    [[nodiscard]] const std::unordered_set<value_type>& allowed_values() const noexcept {
        return allowed_values_;
    }
    void set_option(const value_type& value) noexcept override {
        if (is_allowed(value)) {
            UCIOption<value_type>::set_option(value);
        }
    }

   private:
    std::unordered_set<value_type> allowed_values_;
};

class UCIStringOption : public UCIOption<std::string> {
   public:
    UCIStringOption() : UCIOption<std::string>() {
    }
    UCIStringOption(const std::string& name,
                    const value_type& value,
                    const std::function<void(const value_type&)>& handler)
        : UCIOption<value_type>(name, value, handler) {
    }
};

class UCICheckOption : public UCIOption<bool> {
   public:
    UCICheckOption() : UCIOption<bool>() {
    }
    UCICheckOption(const std::string& name,
                   const value_type& value,
                   const std::function<void(const value_type&)>& handler)
        : UCIOption<value_type>(name, value, handler) {
    }
};

class UCIButtonOption : public UCIOption<void> {
   public:
    UCIButtonOption() : UCIOption<void>() {
    }
    UCIButtonOption(const std::string& name, const std::function<void(void)>& handler) noexcept
        : UCIOption<value_type>(name, handler) {
    }
};

}  // namespace libchess

#endif  // LIBCHESS_UCIOPTION_H
