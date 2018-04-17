Pro Office Calculator
=====================

## To compile

### Linux

To install development dependencies run

```
    sudo apt-get install -y \
      g++ \
      cmake \
      qtbase5-dev \
      qtmultimedia5-dev \
      libqt5multimedia5-plugins \
      libtinyxml2-dev \
      packaging-dev
```

If you don't want to install the development dependencies on your system, you can use the supplied
VagrantFile. From vagrant/xenial run

```
    vagrant up
    vagrant ssh
```

The project directory will be mounted at /vagrant.

gtest can be built from the dependencies directory. Create the directory dependencies/build/linux
and from there run

```
    cmake -DCMAKE_INSTALL_PREFIX=./dist -G "Unix Makefiles" ../..
    make -j4
```

Create the directory build/linux under the project root and from there run

```
    cmake -DCMAKE_INSTALL_PREFIX=./dist/usr -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../..
    make -j4 && make install
```

To run the app, from build/linux run

```
    ./run.sh
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

... making sure they're both on PATH.

If not building Qt 5.10.1 from source (see below), install it via installer from official website.


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

### Linux (Debian)

On the branch you want to release, create a tarball with

```
    ./create_tarball.sh -s
```

It should appear in the build directory. Copy it to outside the project root.

Checkout the debian branch. Create a new changelog file, giving the version number from the tarball
with the debian revision number appended, e.g.

```
    dch --create -v 0.0.1+2SNAPSHOT20180416185206-0ubuntu1 --package procalc
```

... and edit the file so it looks something like this

```
    procalc (0.0.1+2SNAPSHOT20180416185206-0ubuntu1) xenial; urgency=medium

      [Rob Jinman]
      * Initial release.

     -- Rob Jinman <jinmanr@gmail.com>  Mon, 16 Apr 2018 19:55:28 +0100

```

Create a debian package, from the project root, run

```
    debuild
```

To build only the source package for uploading to a launchpad PPA, run

```
    debuild -S -sa -kXXXXXXXX
```

... where -S signifies source only, -sa forces inclusion of the orig tarball, and XXXXXXXX
denotes a PGP key ID. To create a PGP key, install seahorse from the repositories and import to
launchpad.

Upload source package to PPA via launchpad website.


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
