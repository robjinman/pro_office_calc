#!/bin/bash

if [ ! -d src ]; then
  echo "Please run build_deb.sh from project root. Aborting"
  exit 1
fi

source ./release/functions.sh

release=false
source_only=false
snapshot=false

while getopts "k:rsx" opt
do
  case $opt in
    r) release=true
       echo "Making release package"
       ;;
    s) source_only=true
       echo "Building source package only"
       ;;
    k) key_id="$OPTARG"
       ;;
    x) snapshot=true
      ;;
  esac
done

if [ -z $key_id ]; then
  echo "Missing required option -k key_id"
  exit 1
fi

export DEBEMAIL="jinmanr@gmail.com"
export DEBFULLNAME="Rob Jinman"

changelog_path="$(pwd)/debian/changelog"

if [ $snapshot == true ]; then
  ./release/create_tarball.sh -s -v "$version"
else
  ./release/create_tarball.sh -v "$version"
fi

tarball_path="$(get_tarball_path ./build)"
tarball_name="$(get_tarball_name ./build)"
version="$(get_tarball_version ./build)"
deb_version="${version}-0ubuntu1"

mv "$tarball_path" ../
cd ..
tar -xf "$tarball_name"
cd "procalc-${version}"

if $release; then
  dch --release
else
  dch -v "$deb_version"
fi

# Replace the old changelog with the new one
cp ./debian/changelog "$changelog_path"

export CMAKE_BUILD_TYPE=Release

if $source_only; then
  debuild -S -sa -k$key_id
else
  debuild -k$key_id
fi
