#!/bin/sh
mkdir -p test_build
cd test_build
cmake ..
make
make test
./perft/perft perft/perts.epd 6
