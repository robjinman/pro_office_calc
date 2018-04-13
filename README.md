Pro Office Calculator
=====================

## To compile

### Linux

```
    mkdir build
    cd build
    cmake -G "Unix Makefiles" ..
    make
```

### Windows

Download a Windows development VM from
https://developer.microsoft.com/en-us/windows/downloads/virtual-machines.

... which should already contain

  * Visual Studio 15 2017

You will need to install

  * git
  * cmake

If compiling Qt, install

  * Python 2
  * Active Perl

... making sure python and perl are on PATH.

#### Build Qt5 (Optional)

Download Qt source and extract to `C:\Qt\5.10.1\Src`, or install Qt and its sources via installer
from website. Create directories `C:\Qt\5.10.1\build` and `C:\Qt\5.10.1\custom_dist`.

Open the x64 Native Tools command prompt, and navigate to `C:\Qt\5.10.1\build`. Procalc needs at
least qtimageformats, qtmultimedia, and qttools, so delete their skip options from the following
command before running it.

```
    ..\Src\configure -release -nomake examples -nomake tests -nomake tools -opensource -opengl desktop -prefix C:\Qt\5.10.1\custom_dist -skip qt3d -skip qtactiveqt -skip qtandroidextras -skip qtcanvas3d -skip qtconnectivity -skip qtdeclarative -skip qtdoc -skip qtenginio -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation -skip qtmacextras -skip qtmultimedia -skip qtquick1 -skip qtquickcontrols -skip qtscript -skip qtsensors -skip qtserialport -skip qtsvg -skip qttools -skip qttranslations -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebkit -skip qtwebkit-examples -skip qtwebsockets -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns
```

You can specify multimedia backend with `-mediaplayer-backend wmf` (default in Qt 5.10.1) or
`-mediaplayer-backend dsengine`.

Then run `nmake` followed by `nmake install`.


#### Build Pro Office Calculator

Set the QT_DIR variable in CMakelists.txt to the Qt installation directory.

Open the x64 Native Tools command prompt. Build the dependencies first by creating a the directory
dependencies/build/win64 and from there running

```
    cmake -DCMAKE_CXX_FLAGS=/w ^
          -DCMAKE_INSTALL_PREFIX=./dist ^
          -G "Visual Studio 15 2017 Win64" ../..
```

Open the MSVC project and build the Release configuration.

Now create a build/win64 directory under the project root and from there run

```
    cmake -DCMAKE_INSTALL_PREFIX=./dist ^
          -DCMAKE_BUILD_TYPE=Debug ^
          -G "Visual Studio 15 2017 Win64" ../..
```

Open the MSVC project and build solutions ALL_BUILD followed by INSTALL making sure to select the
Release configuration (even if Debug was given for CMAKE_BUILD_TYPE).

Run windeployqt to copy dependencies into bin folder.

```
    C:\Qt\5.10.1\msvc2017_64\bin\windeployqt.exe --release dist\bin\procalc.exe`
```


## Creating distributable packages

### Windows

```
    heat dir "data" -cg dataComponentGroup -ke -srd -dr dataFolder -sfrag -o "windows\data.wxs"
```

## To profile (Linux only)

### Time profile

```
    cmake -G "Unix Makefiles" -DPROFILING_ON=1 ..
    make
    CPUPROFILE=./prof.out ./procalc
    google-pprof --text ./procalc ./prof.out > ./prof.txt
```

For graphical output

```
    google-pprof --gv ./procalc ./prof.out
```

### Cache profile

Build as normal (without PROFILING_ON set), then run with valgrind

```
    valgrind --tool=cachegrind ./procalc
    cg_annotate ./cachegrind.out.1234 > ./cacheprof.txt
```

Supply `--auto=yes` option for annotated source code.
