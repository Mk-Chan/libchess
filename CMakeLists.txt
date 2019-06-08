cmake_minimum_required(VERSION 3.13)

project(libchess)

# Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# Subdirs
add_subdirectory(lib/Catch2)

# Targets
add_executable(libchess_test tests/Test.cpp tests/Color.cpp tests/Bitboard.cpp tests/Piece.cpp tests/PieceType.cpp tests/Move.cpp tests/CastlingRights.cpp tests/Position.cpp)
add_executable(libchess_perft perft/Perft.cpp)

# Linked libs
target_link_libraries(libchess_test Catch2::Catch2)
