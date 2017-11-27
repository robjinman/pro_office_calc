Pro Office Calculator
=====================

## To compile

        mkdir build
        cd build
        cmake -G "Unix Makefiles" ..
        make

## To profile

### Time profile

        cmake -G "Unix Makefiles" -DPROFILING_ON=1 ..
        make
        CPUPROFILE=./prof.out ./procalc 12
        google-pprof --text ./procalc ./prof.out > ./prof.txt

For graphical output

        google-pprof --gv ./procalc ./prof.out

### Cache profile

Build as normal (without PROFILING_ON set), then run with valgrind

        valgrind --tool=cachegrind ./procalc 12
        cg_annotate ./cachegrind.out.1234 > ./cacheprof.txt

Supply `--auto=yes` option for annotated source code.
