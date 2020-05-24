#ifndef LIBCHESS_COLOR_H
#define LIBCHESS_COLOR_H

#include <iostream>
#include <optional>

#include "internal/MetaValueType.h"

namespace libchess {

class Color : public MetaValueType<int> {
   public:
    class Value {
       public:
        enum ColorValue : value_type
        {
            WHITE = 0,
            BLACK = 1
        };
    };
    constexpr explicit Color(value_type value) : MetaValueType<value_type>(value) {
    }

    constexpr Color operator!() {
        return Color{!value()};
    }

    constexpr char to_char() const {
        switch (value()) {
            case Value::WHITE:
                return 'w';
            case Value::BLACK:
                return 'b';
            default:
                return '-';
        }
    }

    constexpr static std::optional<Color> from(char c) {
        switch (c) {
            case 'w':
                return Color{Value::WHITE};
            case 'b':
                return Color{Value::BLACK};
            case 'W':
                return Color{Value::WHITE};
            case 'B':
                return Color{Value::BLACK};
            default:
                return {};
        }
    }
};

inline std::ostream& operator<<(std::ostream& ostream, Color color) {
    return ostream << color.to_char();
}

namespace constants {

constexpr static Color WHITE{Color::Value::WHITE};
constexpr static Color BLACK{Color::Value::BLACK};
constexpr static Color COLORS[]{WHITE, BLACK};

}  // namespace constants

}  // namespace libchess

namespace std {

template <>
struct hash<libchess::Color> : public hash<libchess::Color::value_type> {};

}  // namespace std

#endif  // LIBCHESS_COLOR_H
