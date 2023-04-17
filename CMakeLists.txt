cmake_minimum_required(VERSION 3.12)

project(libchess)

# Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Ofast")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif ()

enable_testing()

# Subdirs
add_subdirectory(lib/Catch2)
add_subdirectory(tests)
add_subdirectory(perft)
#add_subdirectory(misc)
