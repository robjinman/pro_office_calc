#!/bin/bash

cd dist
LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:./usr/lib" ./usr/bin/procalc "$@"
