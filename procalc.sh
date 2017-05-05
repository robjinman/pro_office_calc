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
  hangmanAppName="Hangman.app"

  # Check /Applications and then ~/Applications for Hangman.app
  if [ -x "/Applications/$hangmanAppName" ]; then
    hangmanDir="/Applications"
  elif [ -x "$HOME/Applications/$hangmanAppName" ]; then
    hangmanDir="$HOME/Applications"
  else
    # Exit if Hangman can't be found
    if [ ! -x "$hangmanDir/$hangmanAppName" ]; then
      printf "Cannot locate Hangman.app, it is usually located in /Applications.\n"
      exit 1
    fi
  fi

  open -a "$hangmanPath/$hangmanAppName" -n --args "$@"
elif [ $systemName == "Linux" ]; then
  # This script should be at usr/games
  script=$(readlink -f "$0")
  usrDir=$(readlink -f "$(dirname $script)/..")

  (cd "$usrDir/share/hangman"; nohup "$usrDir/bin/hangman" "$@" > /dev/null 2>&1) &
fi
