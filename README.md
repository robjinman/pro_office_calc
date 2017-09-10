Pro Office Calculator
=====================

To compile

        mkdir build
        cd build
        cmake -G "Unix Makefiles" ..
        make

To profile

        cmake -G "Unix Makefiles" -DPROFILING_ON ..
        make
        CPUPROFILE=./prof.out ./procalc 12
        google-pprof --text ./procalc ./prof.out > ./prof.txt
