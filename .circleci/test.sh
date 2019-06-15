#!/bin/sh
wget https://github.com/Kitware/CMake/releases/download/v3.14.3/cmake-3.14.3-Linux-x86_64.sh \
    -q -O /tmp/cmake-install.sh \
    && chmod u+x /tmp/cmake-install.sh \
    && mkdir /usr/bin/cmake \
    && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
    && rm /tmp/cmake-install.sh
export PATH=${PATH}:/usr/bin/cmake/bin/

mkdir -p test_build
cd test_build
cmake ..
make
make test
./perft/perft perft/perts.epd 6
