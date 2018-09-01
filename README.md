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
      packaging-dev \
      chrpath \
      patchelf
```

And install linuxdeployqt from https://github.com/probonopd/linuxdeployqt.

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
    cmake -D CMAKE_INSTALL_PREFIX=./dist -G "Unix Makefiles" ../..
    make -j4
```

Create the directory build/linux under the project root and from there run

```
    cmake -D CMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../..
    make -j4
```

Run the app from the build directory

```
    ./procalc
```

To create a standalone release bundle, invoke cmake as follows

```
    # Remove existing bundle
    rm -r ../../dist/bundles/linux

    cmake -D CMAKE_BUILD_TYPE=Release \
          -D CMAKE_INSTALL_PREFIX=../../dist/bundles/linux/procalc \
          -G "Unix Makefiles" ../..

    make -j4
    make install

    # So the app will run on systems with older version of libstdc++.so (e.g. SteamOS)
    cp /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.21 ../../dist/bundles/linux/procalc/lib/libstdc++.so.6
```

And use [linuxdeployqt](https://github.com/probonopd/linuxdeployqt) to copy the dependencies into
place.

```
    linuxdeployqt ../../dist/bundles/linux/procalc/share/applications/procalc.desktop \
                  -bundle-non-qt-libs
```

This will set the executable's RUNPATH to `$ORIGIN/../lib`, but for Steam we need this to be the
RPATH instead. To fix this, run the following

```
    chrpath -d -r "" ../../dist/bundles/linux/procalc/bin/procalc
    patchelf --set-rpath '$ORIGIN/../lib' --force-rpath ../../dist/bundles/linux/procalc/bin/procalc
```


### OS X

Install the development dependencies via homebrew

```
    brew install qt
    brew install tinyxml2
    brew install llvm
```

Create the directory build/osx and from there, run

```
    CC=/usr/local/opt/llvm/bin/clang CXX=/usr/local/opt/llvm/bin/clang++ cmake \
      -D CMAKE_BUILD_TYPE=Debug \
      -G "Unix Makefiles" ../..

    make -j4
    make install
```

This will create a redistributable app bundle called procalc.app.

For the release build, do the following

```
    CC=/usr/local/opt/llvm/bin/clang CXX=/usr/local/opt/llvm/bin/clang++ cmake \
      -D CMAKE_BUILD_TYPE=Release \
      -G "Unix Makefiles" ../..

    make -j4
    make install
```


### Windows

Download a Windows development VM from
https://developer.microsoft.com/en-us/windows/downloads/virtual-machines.

... which should already contain

  * Visual Studio 15 2017

You will need to install

  * git
  * cmake
  * wix toolset

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
    ..\Src\configure -release -nomake examples -nomake tests -nomake tools -opensource ^
      -opengl desktop -prefix C:\Qt\5.10.1\custom_dist -skip qt3d -skip qtactiveqt ^
      -skip qtandroidextras -skip qtcanvas3d -skip qtconnectivity -skip qtdeclarative -skip qtdoc ^
      -skip qtenginio -skip qtgraphicaleffects -skip qtimageformats -skip qtlocation ^
      -skip qtmacextras -skip qtmultimedia -skip qtquick1 -skip qtquickcontrols -skip qtscript ^
      -skip qtsensors -skip qtserialport -skip qtsvg -skip qttools -skip qttranslations ^
      -skip qtwayland -skip qtwebchannel -skip qtwebengine -skip qtwebkit -skip qtwebkit-examples ^
      -skip qtwebsockets -skip qtwinextras -skip qtx11extras -skip qtxmlpatterns
```

You can specify multimedia backend with `-mediaplayer-backend wmf` (default in Qt 5.10.1) or
`-mediaplayer-backend dsengine`.

Then run `nmake` followed by `nmake install`.


#### Build Pro Office Calculator

Set the QT_DIR variable in CMakelists.txt to the Qt installation directory.

Open the x64 Native Tools command prompt. Build the dependencies first by creating a the directory
dependencies/build/win64 and from there running

```
    cmake -D CMAKE_CXX_FLAGS=/w ^
          -D CMAKE_INSTALL_PREFIX=./dist ^
          -G "Visual Studio 15 2017 Win64" ../..
```

Open the MSVC project and build the Release configuration.

Now create a build/win64 directory under the project root and from there run the following

For the debug build

```
    cmake -D CMAKE_INSTALL_PREFIX=./dist ^
          -D CMAKE_BUILD_TYPE=Debug ^
          -G "Visual Studio 15 2017 Win64" ../..
```

Open the MSVC project and build the INSTALL solution making sure to select the Release
configuration (even if Debug was given for CMAKE_BUILD_TYPE).

Run windeployqt to copy dependencies into bin folder.

```
    C:\Qt\5.10.1\msvc2017_64\bin\windeployqt.exe --release dist\procalc.exe --dir dist\libs
```

To run the app, from build/win64/dist

```
    run
```

For the release build, the relevant commands are

```
    cmake -D CMAKE_INSTALL_PREFIX=../../dist/bundles/windows/procalc ^
          -D CMAKE_BUILD_TYPE=Release ^
          -G "Visual Studio 15 2017 Win64" ../..
```
```
    C:\Qt\5.10.1\msvc2017_64\bin\windeployqt.exe ^
      --release ..\..\dist\bundles\windows\procalc\procalc.exe ^
      --dir ..\..\dist\bundles\windows\procalc\libs
```


## Creating distributable packages

Putting this here for my own convenience as maintainer.


### Linux (Debian)

On the branch you want to release, run

```
    ./release/build_deb.sh -rsx -k XXXXXX
```

... where the options are as follows

```
    -r         Signifies a release build. Only affects the changelog by causing dch to be run with
               --release option
    -x         Stable release (as opposed to snapshot release)
    -s         Build source package only
    -k XXXXXX  The ID of the PGP key to sign the package with
```

To create a PGP key, use seahorse (a GnuPG front-end, which can be installed from the repositories),
and add the key to launchpad.

If it fails to sign, it could be that seahorse was using gpg2 to create the key, but debuild is
using gpg1. A workaround is to create a symlink at /usr/bin/gpg pointing to /usr/bin/gpg2, e.g.

```
    sudo mv /usr/bin/gpg /usr/bin/gpg.bak
    sudo ln -s /usr/bin/gpg2 /usr/bin/gpg
```

Upload the source package to the PPA with dput, specifying the .changes file, e.g.

```
    dput ppa:rjinman/snapshots ../procalc_0.0.1+2SNAPSHOT20180417201730-0ubuntu1_source.changes
```

Users can now install Pro Office Calculator by adding the PPA and running apt-get.

```
    sudo add-apt-repository ppa:rjinman/snapshots
    sudo apt-get update
    sudo apt-get install procalc
```

Note that the above is performed by the release script, so it may be unnecessary to run build_deb.sh
directly. See the [releases readme](release/README.md) for more information.


### Windows

The data.wxs was initially created by running (from the windows dir)

```
    heat dir "..\dist\bundles\windows\procalc\data" ^
      -cg dataComponentGroup -ke -gg -srd -dr dataFolder -sfrag -o "data.wxs"
```

... and libs.wxs with

```
    heat dir "..\dist\bundles\windows\procalc\libs" ^
      -cg libsComponentGroup -ke -gg -srd -dr INSTALLDIR -sfrag -o "libs.wxs"
```

... where the arguments mean

```
    -cg     Create a component group that we can refer to from procalc.wxs
    -srd    Suppress identifier generation of the root folder (we want to use /data as the root)
    -dr     Specify the directory name
    -sfrag  Suppress fragment generation
    -ke     Keep empty directories
    -gg     Generate UUIDs
```

It is unlikely the above commands will need to be run again as these wxs files should now be
maintained manually.

```
    candle procalc.wxs libs.wxs data.wxs -arch x64

    light procalc.wixobj data.wixobj libs.wixobj ^
      -b ..\dist\bundles\windows\procalc ^
      -b ..\dist\bundles\windows\procalc\libs ^
      -b ..\dist\bundles\windows\procalc\data ^
      -o procalc.msi
```


## To profile (Linux only)

### Time profile

```
    cmake -D CMAKE_BUILD_TYPE=Debug -D PROFILING_ON=1 -G "Unix Makefiles" ../..
    make -j4
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
