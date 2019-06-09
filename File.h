#ifndef LIBCHESS_FILE_H
#define LIBCHESS_FILE_H

#include <cstdint>

#include "internal/MetaValueType.h"

namespace libchess {

class File : public MetaValueType<std::uint8_t> {
  public:
    class Value {
      public:
        enum FileValue : value_type {
            FILE_A,
            FILE_B,
            FILE_C,
            FILE_D,
            FILE_E,
            FILE_F,
            FILE_G,
            FILE_H
        };
    };
    constexpr inline File(value_type value) : MetaValueType<value_type>(value) {}

    constexpr inline char to_char() const { return 'a' + value(); }

    constexpr static inline File from(char c) {
        int offset = c >= 'a' ? (c - 'a') : (c - 'A');
        return Value::FILE_A + offset;
    }
};

namespace constants {

constexpr inline File FILE_A = File{File::Value::FILE_A};
constexpr inline File FILE_B = File{File::Value::FILE_B};
constexpr inline File FILE_C = File{File::Value::FILE_C};
constexpr inline File FILE_D = File{File::Value::FILE_D};
constexpr inline File FILE_E = File{File::Value::FILE_E};
constexpr inline File FILE_F = File{File::Value::FILE_F};
constexpr inline File FILE_G = File{File::Value::FILE_G};
constexpr inline File FILE_H = File{File::Value::FILE_H};

} // namespace constants

} // namespace libchess

namespace std {

template <> struct hash<libchess::File> : public hash<libchess::File::value_type> {};

} // namespace std

#endif // LIBCHESS_FILE_H
