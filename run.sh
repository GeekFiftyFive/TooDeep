#!/bin/bash
set -e
echo Setting Up
mkdir -p obj
echo Compiling
make
echo Running unit tests
./tooDeep -run-tests
echo Launching game
./tooDeep examples/spaceship