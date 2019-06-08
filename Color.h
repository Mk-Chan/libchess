#ifndef LIBCHESS_COLOR_H
#define LIBCHESS_COLOR_H

#include <iostream>

#include "internal/MetaValueType.h"

namespace libchess {

class Color : public MetaValueType<int> {
  public:
    class Value {
      public:
        enum ColorValue : value_type { WHITE = 0, BLACK = 1, NUM_COLORS, NONE };
    };
    constexpr inline Color(value_type value) : MetaValueType<value_type>(value) {}
    constexpr inline Color() : Color(Value::NONE) {}

    constexpr inline char to_char() const {
        switch (value()) {
        case Value::WHITE:
            return 'w';
        case Value::BLACK:
            return 'b';
        default:
            return '-';
        }
    }

    constexpr inline static Color from(char c) {
        switch (c) {
        case 'w':
            return Value::WHITE;
        case 'b':
            return Value::BLACK;
        case 'W':
            return Value::WHITE;
        case 'B':
            return Value::BLACK;
        default:
            return Value::NONE;
        }
    }
};

inline std::ostream& operator<<(std::ostream& ostream, Color color) {
    return ostream << color.to_char();
}

namespace constants {

constexpr inline Color WHITE{Color::Value::WHITE};
constexpr inline Color BLACK{Color::Value::BLACK};
constexpr inline Color COLOR_NONE{Color::Value::NONE};
constexpr inline Color COLOR_LIST[]{WHITE, BLACK};
constexpr inline int NUM_COLORS = Color::Value::NUM_COLORS;

} // namespace constants

} // namespace libchess

#endif // LIBCHESS_COLOR_H
