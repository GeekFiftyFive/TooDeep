#!/bin/bash
set -e
echo Setting Up
mkdir -p obj
echo Compiling
OS=`uname -s`
if [ "$OS" = "Darwin" ]; then
    make -j$(sysctl -n hw.ncpu)
else
    make -j$(nproc)
fi
echo Running unit tests
./tooDeep -run-tests
echo Launching game
./tooDeep examples/spaceship