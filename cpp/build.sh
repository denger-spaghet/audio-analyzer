#!/bin/bash

width=$(tput cols)

# Create build directory
mkdir -p build
cd build

# Run CMake to configure the project
cmake ..

# Build the project
cmake --build .

# if -s argument is used start the binary 
if [ $# -eq 1 ]; then
    if [ "$1" = "-s" ]; then
        printf "%${width}s\n" | tr " " "="
        ./bin/audio_analyzer
    fi
fi