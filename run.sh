#!/bin/bash
set -e
echo Setting Up
mkdir -p obj
echo Compiling
make -j$(nproc)
echo Running unit tests
./tooDeep -run-tests
echo Launching game
./tooDeep examples/spaceship