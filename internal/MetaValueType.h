#ifndef LIBCHESS_METAVALUETYPE_H
#define LIBCHESS_METAVALUETYPE_H

template <class T> class MetaValueType {
  public:
    using value_type = T;

    constexpr inline MetaValueType(value_type value) : value_(value) {}

    constexpr inline operator value_type() const { return value_; }

    constexpr inline value_type operator+=(value_type val) {
        const value_type ret = value();
        set_value(value() + val);
        return ret;
    }
    constexpr inline value_type operator-=(value_type val) {
        const value_type ret = value();
        set_value(value() - val);
        return ret;
    }
    constexpr inline value_type operator++() {
        set_value(value() + 1);
        return value();
    }
    constexpr inline const value_type operator++(int) {
        const value_type ret = value();
        set_value(value() + 1);
        return ret;
    }
    constexpr inline value_type operator--() {
        set_value(value() - 1);
        return value();
    }
    constexpr inline const value_type operator--(int) {
        const value_type ret = value();
        set_value(value() - 1);
        return ret;
    }

    constexpr inline value_type value() const { return value_; }

  protected:
    constexpr inline void set_value(const value_type& value) { value_ = value; }

  private:
    value_type value_;
};

#endif // LIBCHESS_METAVALUETYPE_H
