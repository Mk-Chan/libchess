cmake_minimum_required(VERSION 3.12)

# Targets
configure_file(perfts.epd perfts.epd COPYONLY)
add_executable(perft Perft.cpp)
