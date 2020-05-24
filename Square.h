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
        };
        // clang-format off
    };
    constexpr explicit Square(value_type value) : MetaValueType<value_type>(value) {}

    constexpr Square operator+(value_type val) const {
        return Square{value() + val};
    }
    constexpr Square operator-(value_type val) const {
        return Square{value() - val};
    }
    constexpr Square& operator=(value_type val) {
        set_value(val);
        return *this;
    }

    constexpr File file() const { return File{value() & 7}; }
    constexpr Rank rank() const { return Rank{value() >> 3}; }
    constexpr Square flipped() const { return Square{value() ^ 56}; }

    std::string to_str() const {
        return std::string{file().to_char(), rank().to_char()};
    }

    constexpr static std::optional<Square> from(std::optional<File> file, std::optional<Rank> rank) {
        if (!(file && rank)) {
            return {};
        }
        return Square{*file | (*rank << 3)};
    }
    static std::optional<Square> from(const std::string& square_str) {
        return Square::from(File::from(square_str[0]), Rank::from(square_str[1]));
    }
};

inline std::ostream& operator<<(std::ostream& ostream, Square square) {
    return ostream << square.to_str();
}

namespace constants {

constexpr static Square A1{Square::Value::A1};
constexpr static Square A2{Square::Value::A2};
constexpr static Square A3{Square::Value::A3};
constexpr static Square A4{Square::Value::A4};
constexpr static Square A5{Square::Value::A5};
constexpr static Square A6{Square::Value::A6};
constexpr static Square A7{Square::Value::A7};
constexpr static Square A8{Square::Value::A8};

constexpr static Square B1{Square::Value::B1};
constexpr static Square B2{Square::Value::B2};
constexpr static Square B3{Square::Value::B3};
constexpr static Square B4{Square::Value::B4};
constexpr static Square B5{Square::Value::B5};
constexpr static Square B6{Square::Value::B6};
constexpr static Square B7{Square::Value::B7};
constexpr static Square B8{Square::Value::B8};

constexpr static Square C1{Square::Value::C1};
constexpr static Square C2{Square::Value::C2};
constexpr static Square C3{Square::Value::C3};
constexpr static Square C4{Square::Value::C4};
constexpr static Square C5{Square::Value::C5};
constexpr static Square C6{Square::Value::C6};
constexpr static Square C7{Square::Value::C7};
constexpr static Square C8{Square::Value::C8};

constexpr static Square D1{Square::Value::D1};
constexpr static Square D2{Square::Value::D2};
constexpr static Square D3{Square::Value::D3};
constexpr static Square D4{Square::Value::D4};
constexpr static Square D5{Square::Value::D5};
constexpr static Square D6{Square::Value::D6};
constexpr static Square D7{Square::Value::D7};
constexpr static Square D8{Square::Value::D8};

constexpr static Square E1{Square::Value::E1};
constexpr static Square E2{Square::Value::E2};
constexpr static Square E3{Square::Value::E3};
constexpr static Square E4{Square::Value::E4};
constexpr static Square E5{Square::Value::E5};
constexpr static Square E6{Square::Value::E6};
constexpr static Square E7{Square::Value::E7};
constexpr static Square E8{Square::Value::E8};

constexpr static Square F1{Square::Value::F1};
constexpr static Square F2{Square::Value::F2};
constexpr static Square F3{Square::Value::F3};
constexpr static Square F4{Square::Value::F4};
constexpr static Square F5{Square::Value::F5};
constexpr static Square F6{Square::Value::F6};
constexpr static Square F7{Square::Value::F7};
constexpr static Square F8{Square::Value::F8};

constexpr static Square G1{Square::Value::G1};
constexpr static Square G2{Square::Value::G2};
constexpr static Square G3{Square::Value::G3};
constexpr static Square G4{Square::Value::G4};
constexpr static Square G5{Square::Value::G5};
constexpr static Square G6{Square::Value::G6};
constexpr static Square G7{Square::Value::G7};
constexpr static Square G8{Square::Value::G8};

constexpr static Square H1{Square::Value::H1};
constexpr static Square H2{Square::Value::H2};
constexpr static Square H3{Square::Value::H3};
constexpr static Square H4{Square::Value::H4};
constexpr static Square H5{Square::Value::H5};
constexpr static Square H6{Square::Value::H6};
constexpr static Square H7{Square::Value::H7};
constexpr static Square H8{Square::Value::H8};

// clang-format off
constexpr static Square SQUARES[]{
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

}  // namespace constants

}  // namespace libchess

namespace std {

template <>
struct hash<libchess::Square> : public hash<libchess::Square::value_type> {};

}  // namespace std

#endif  // LIBCHESS_SQUARE_H
