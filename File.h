#ifndef LIBCHESS_FILE_H
#define LIBCHESS_FILE_H

#include <cstdint>
#include <optional>

#include "internal/MetaValueType.h"

namespace libchess {

class File : public MetaValueType<int> {
   public:
    class Value {
       public:
        enum FileValue : value_type
        {
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
    constexpr explicit File(value_type value) : MetaValueType<value_type>(value) {
    }

    constexpr char to_char() const {
        return 'a' + value();
    }

    constexpr static std::optional<File> from(char c) {
        int offset = c >= 'a' ? (c - 'a') : (c - 'A');
        if (offset < 0 || offset > 7) {
            return {};
        }
        return File{Value::FILE_A + offset};
    }
};

namespace constants {

constexpr static File FILE_A = File{File::Value::FILE_A};
constexpr static File FILE_B = File{File::Value::FILE_B};
constexpr static File FILE_C = File{File::Value::FILE_C};
constexpr static File FILE_D = File{File::Value::FILE_D};
constexpr static File FILE_E = File{File::Value::FILE_E};
constexpr static File FILE_F = File{File::Value::FILE_F};
constexpr static File FILE_G = File{File::Value::FILE_G};
constexpr static File FILE_H = File{File::Value::FILE_H};

}  // namespace constants

}  // namespace libchess

namespace std {

template <>
struct hash<libchess::File> : public hash<libchess::File::value_type> {};

}  // namespace std

#endif  // LIBCHESS_FILE_H
