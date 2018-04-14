#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
  systemName="Mac"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  systemName="Linux"
else
  printf "Your platform ($(uname -a)) is not supported.\n"
  exit 1
fi

if [ "$systemName" == "Mac" ]; then
  procalcAppName="procalc.app"

  # Check /Applications and then ~/Applications for procalc.app
  if [ -x "/Applications/$procalcAppName" ]; then
    procalcDir="/Applications"
  elif [ -x "$HOME/Applications/$procalcAppName" ]; then
    procalcDir="$HOME/Applications"
  else
    # Exit if procalc can't be found
    if [ ! -x "$procalcDir/$procalcAppName" ]; then
      printf "Cannot locate procalc.app, it is usually located in /Applications.\n"
      exit 1
    fi
  fi

  open -a "$procalcPath/$procalcAppName" -n --args "$@"
elif [ $systemName == "Linux" ]; then
  # This script should be at usr/games
  script=$(readlink -f "$0")
  usrDir=$(readlink -f "$(dirname $script)/..")

  (cd "$usrDir/share/procalc"; nohup "$usrDir/bin/procalc" "$@" > /dev/null 2>&1) &
fi
