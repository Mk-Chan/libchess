#!/bin/sh
mkdir -p test_build
cd test_build
cmake ..
make
make test
./perft/perft ./perft/perfts.epd 6
