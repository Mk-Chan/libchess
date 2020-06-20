cmake_minimum_required(VERSION 3.12)

project(libchess)

# Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

enable_testing()

# Subdirs
add_subdirectory(lib/Catch2)
add_subdirectory(tests)
add_subdirectory(perft)
#add_subdirectory(misc)
