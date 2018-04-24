#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 [OPTIONS] keyid"
  exit 1
fi

release=false
source_only=false

while getopts "k:rs" opt
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
  esac
done

if [ -z $key_id ]; then
  echo "Missing required option -k XXXXXX"
  exit 1
fi

export DEBEMAIL="jinmanr@gmail.com"
export DEBFULLNAME="Rob Jinman"

changelog_path="$(pwd)/debian/changelog"

./create_tarball.sh -s

tarball_path=$(find build -name "procalc_*.orig.tar.gz")
tarball_name="${tarball_path##*/}"

regex="^procalc_(.*)\.orig\.tar\.gz$"

if [[ ! $tarball_name =~ $regex ]]; then
  echo "Tarball has non-conforming file name"
  exit 1
fi

version="${BASH_REMATCH[1]}"
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
