cmake_minimum_required(VERSION 3.12)

# Targets
add_executable(libchess_test Tests.cpp ColorTests.cpp BitboardTests.cpp PieceTests.cpp PieceTypeTests.cpp MoveTests.cpp CastlingRightsTests.cpp PositionTests.cpp UCIServiceTests.cpp)

# Linked libs
target_link_libraries(libchess_test Catch2::Catch2)

# Tests
add_test(libchess_test_build "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target libchess_test)
add_test(libchess_test_run libchess_test)
set_tests_properties(libchess_test_run PROPERTIES DEPENDS libchess_test_build)
