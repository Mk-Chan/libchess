#ifndef LIBCHESS_SQUARE_H
#define LIBCHESS_SQUARE_H

#include <cstdint>
#include <string>

#include "File.h"
#include "Rank.h"

#include "internal/MetaValueType.h"

namespace libchess {

class Square : public MetaValueType<int> {
  public:
    class Value {
      public:
        // clang-format off
        enum SquareValue : value_type {
            A1, B1, C1, D1, E1, F1, G1, H1,
            A2, B2, C2, D2, E2, F2, G2, H2,
            A3, B3, C3, D3, E3, F3, G3, H3,
            A4, B4, C4, D4, E4, F4, G4, H4,
            A5, B5, C5, D5, E5, F5, G5, H5,
            A6, B6, C6, D6, E6, F6, G6, H6,
            A7, B7, C7, D7, E7, F7, G7, H7,
            A8, B8, C8, D8, E8, F8, G8, H8,
            NUM_SQUARES,
            SQUARE_INVALID
        };
        // clang-format off
    };
    constexpr inline Square(value_type value) : MetaValueType<value_type>(value) {}
    constexpr inline Square() : Square(Value::SQUARE_INVALID) {}

    constexpr inline Square& operator=(value_type val) {
        set_value(val);
        return *this;
    }

    constexpr inline File file() const { return value() & 7; }
    constexpr inline Rank rank() const { return value() >> 3; }

    inline std::string to_str() const {
        if (value() == Value::SQUARE_INVALID) {
            return "-";
        }
        return std::string{file().to_char(), rank().to_char()};
    }

    constexpr static inline Square from(File file, Rank rank) {
        return file.value() | (rank.value() << 3);
    }
    inline static Square from(const std::string& square_str) {
        if (square_str == "-") {
            return Value::SQUARE_INVALID;
        }
        return Square::from(File::from(square_str[0]), Rank::from(square_str[1]));
    }
};

inline std::ostream& operator<<(std::ostream& ostream, Square square) {
    return ostream << square.to_str();
}

namespace constants {

constexpr inline Square A1{Square::Value::A1};
constexpr inline Square A2{Square::Value::A2};
constexpr inline Square A3{Square::Value::A3};
constexpr inline Square A4{Square::Value::A4};
constexpr inline Square A5{Square::Value::A5};
constexpr inline Square A6{Square::Value::A6};
constexpr inline Square A7{Square::Value::A7};
constexpr inline Square A8{Square::Value::A8};

constexpr inline Square B1{Square::Value::B1};
constexpr inline Square B2{Square::Value::B2};
constexpr inline Square B3{Square::Value::B3};
constexpr inline Square B4{Square::Value::B4};
constexpr inline Square B5{Square::Value::B5};
constexpr inline Square B6{Square::Value::B6};
constexpr inline Square B7{Square::Value::B7};
constexpr inline Square B8{Square::Value::B8};

constexpr inline Square C1{Square::Value::C1};
constexpr inline Square C2{Square::Value::C2};
constexpr inline Square C3{Square::Value::C3};
constexpr inline Square C4{Square::Value::C4};
constexpr inline Square C5{Square::Value::C5};
constexpr inline Square C6{Square::Value::C6};
constexpr inline Square C7{Square::Value::C7};
constexpr inline Square C8{Square::Value::C8};

constexpr inline Square D1{Square::Value::D1};
constexpr inline Square D2{Square::Value::D2};
constexpr inline Square D3{Square::Value::D3};
constexpr inline Square D4{Square::Value::D4};
constexpr inline Square D5{Square::Value::D5};
constexpr inline Square D6{Square::Value::D6};
constexpr inline Square D7{Square::Value::D7};
constexpr inline Square D8{Square::Value::D8};

constexpr inline Square E1{Square::Value::E1};
constexpr inline Square E2{Square::Value::E2};
constexpr inline Square E3{Square::Value::E3};
constexpr inline Square E4{Square::Value::E4};
constexpr inline Square E5{Square::Value::E5};
constexpr inline Square E6{Square::Value::E6};
constexpr inline Square E7{Square::Value::E7};
constexpr inline Square E8{Square::Value::E8};

constexpr inline Square F1{Square::Value::F1};
constexpr inline Square F2{Square::Value::F2};
constexpr inline Square F3{Square::Value::F3};
constexpr inline Square F4{Square::Value::F4};
constexpr inline Square F5{Square::Value::F5};
constexpr inline Square F6{Square::Value::F6};
constexpr inline Square F7{Square::Value::F7};
constexpr inline Square F8{Square::Value::F8};

constexpr inline Square G1{Square::Value::G1};
constexpr inline Square G2{Square::Value::G2};
constexpr inline Square G3{Square::Value::G3};
constexpr inline Square G4{Square::Value::G4};
constexpr inline Square G5{Square::Value::G5};
constexpr inline Square G6{Square::Value::G6};
constexpr inline Square G7{Square::Value::G7};
constexpr inline Square G8{Square::Value::G8};

constexpr inline Square H1{Square::Value::H1};
constexpr inline Square H2{Square::Value::H2};
constexpr inline Square H3{Square::Value::H3};
constexpr inline Square H4{Square::Value::H4};
constexpr inline Square H5{Square::Value::H5};
constexpr inline Square H6{Square::Value::H6};
constexpr inline Square H7{Square::Value::H7};
constexpr inline Square H8{Square::Value::H8};

// clang-format off
constexpr inline Square SQUARES[]{
  A1, B1, C1, D1, E1, F1, G1, H1,
  A2, B2, C2, D2, E2, F2, G2, H2,
  A3, B3, C3, D3, E3, F3, G3, H3,
  A4, B4, C4, D4, E4, F4, G4, H4,
  A5, B5, C5, D5, E5, F5, G5, H5,
  A6, B6, C6, D6, E6, F6, G6, H6,
  A7, B7, C7, D7, E7, F7, G7, H7,
  A8, B8, C8, D8, E8, F8, G8, H8
};
// clang-format on

constexpr inline Square SQUARE_INVALID{Square::Value::SQUARE_INVALID};
constexpr inline int NUM_SQUARES = Square::Value::NUM_SQUARES;

} // namespace constants

} // namespace libchess

#endif // LIBCHESS_SQUARE_H
