#!/usr/bin/env bash

apt-get update
apt-get install -y \
  g++ \
  cmake \
  qtbase5-dev \
  qtmultimedia5-dev \
  libqt5multimedia5-plugins
