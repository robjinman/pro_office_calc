#!/usr/bin/env bash

apt-get update
apt-get install -y \
  g++ \
  cmake \
  qtbase5-dev \
  qtmultimedia5-dev \
  qt5-default \
  libqt5multimedia5-plugins \
  libtinyxml2-dev \
  packaging-dev \
  chrpath \
  patchelf
