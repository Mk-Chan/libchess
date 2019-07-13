#ifndef LIBCHESS_COLOR_H
#define LIBCHESS_COLOR_H

#include <array>
#include <iostream>

#include "internal/MetaValueType.h"

namespace libchess {

/// Represents a color/side to move {White, Black}.
class Color : public MetaValueType<int> {
  public:
    /// Set of possible Color values
    class Value {
      public:
        enum ColorValue : value_type { WHITE = 0, BLACK = 1 };
    };
    constexpr explicit Color(value_type value) noexcept : MetaValueType<value_type>(value) {}

    /// Reverses the color.
    [[nodiscard]] constexpr Color operator!() noexcept { return Color{!value()}; }

    /// The character representation of Color.
    [[nodiscard]] constexpr char to_char() const noexcept {
        switch (value()) {
        case Value::WHITE:
            return 'w';
        case Value::BLACK:
            return 'b';
        default:
            return '-';
        }
    }

    /// Parses Color from a character
    [[nodiscard]] constexpr static std::optional<Color> from(char c) noexcept {
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
constexpr static std::array<Color, 2> COLORS{WHITE, BLACK};

} // namespace constants

} // namespace libchess

namespace std {

template <> struct hash<libchess::Color> : public hash<libchess::Color::value_type> {};

} // namespace std

#endif // LIBCHESS_COLOR_H
