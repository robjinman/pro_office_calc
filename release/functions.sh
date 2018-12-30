#!/bin/bash

get_tarball_path() {
  if [ $# -lt 1 ]; then
    echo "ERROR: Failed to get tarball path. No search path argument given"
    return 1
  fi

  echo "$(find $1 -name procalc_*.orig.tar.gz)"
}

get_tarball_name() {
  if [ $# -lt 1 ]; then
    echo "ERROR: Failed to get tarball path. No search path argument given"
    return 1
  fi

  tarball_path="$(get_tarball_path $1)"
  echo "${tarball_path##*/}"
}

get_tarball_version() {
  if [ $# -lt 1 ]; then
    echo "ERROR: Failed to get tarball path. No search path argument given"
    return 1
  fi

  tarball_name="$(get_tarball_name $1)"
  regex="^procalc_(.*)\.orig\.tar\.gz$"

  if [[ ! "$tarball_name" =~ $regex ]]; then
    echo "ERROR: Tarball does not exist or has non-conforming file name"
    return 1
  fi

  echo "${BASH_REMATCH[1]}"
}
