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

./create_tarball.sh -s

tarball_path=$(find build -name "procalc_*.orig.tar.gz")

mv "${tarball_path}" ../

regex=".*procalc_(.*)\.orig\.tar\.gz$"

if [[ $tarball_path =~ $regex ]]; then
  version="${BASH_REMATCH[1]}"
  deb_version="${version}-0ubuntu1"

  if $release; then
    echo "RELEASE BUILD"
    dch --release
  else
    dch -v "${deb_version}"
  fi

  CMAKE_BUILD_TYPE=Release debuild -k${key_id}
fi
