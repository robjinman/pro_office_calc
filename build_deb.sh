#!/bin/bash

if [ $# -lt 1 ]; then
  echo "Usage: $0 [OPTIONS] keyid"
  exit 1
fi

release=false

while getopts "r" opt
do
  case $opt in
    r) release=true
       ;;
  esac
done

key_id="$1"
export DEBEMAIL="jinmanr@gmail.com"
export DEBFULLNAME="Rob Jinman"

changelog_path="${pwd}/debian/changelog"

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

mv "${tarball_path}" ../
cd ..
tar -xf "${tarball_name}"
cd "procalc-${version}"

if $release; then
  echo "Making release package"
  dch --release
else
  dch -v "${deb_version}"
fi

# Replace the old changelog with the new one
mv ./debian/changelog "${changelog_path}"

export CMAKE_BUILD_TYPE=Release
debuild -k${key_id}
