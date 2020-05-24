#ifndef LIBCHESS_METAVALUETYPE_H
#define LIBCHESS_METAVALUETYPE_H

namespace libchess {

template <class T>
class MetaValueType {
   public:
    using value_type = T;

    constexpr MetaValueType(value_type value) : value_(value) {
    }

    constexpr operator value_type() const {
        return value_;
    }

    constexpr value_type operator+=(value_type val) {
        const value_type ret = value();
        set_value(value() + val);
        return ret;
    }
    constexpr value_type operator-=(value_type val) {
        const value_type ret = value();
        set_value(value() - val);
        return ret;
    }
    constexpr value_type operator++() {
        set_value(value() + 1);
        return value();
    }
    constexpr const value_type operator++(int) {
        const value_type ret = value();
        set_value(value() + 1);
        return ret;
    }
    constexpr value_type operator--() {
        set_value(value() - 1);
        return value();
    }
    constexpr const value_type operator--(int) {
        const value_type ret = value();
        set_value(value() - 1);
        return ret;
    }

    constexpr value_type value() const {
        return value_;
    }

   protected:
    constexpr void set_value(const value_type& value) {
        value_ = value;
    }

   private:
    value_type value_;
};

}  // namespace libchess

#endif  // LIBCHESS_METAVALUETYPE_H
