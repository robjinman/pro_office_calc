#!/bin/bash

mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=./out/usr/local -G "Unix Makefiles" ..
make -j4
make install
