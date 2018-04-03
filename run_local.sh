#!/bin/bash

cd build/out

LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:./usr/local/lib" ./usr/local/bin/procalc "$@"
