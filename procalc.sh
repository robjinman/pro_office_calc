#!/bin/bash

# This script should be at usr/games
script=$(readlink -f "$0")
usrDir=$(readlink -f "$(dirname $script)/..")

(cd "$usrDir/share/procalc"; nohup "$usrDir/bin/procalc" "$@" > /dev/null 2>&1) &
