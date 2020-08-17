#!/bin/bash

# Purge build & bin directories
if [ -d "build" ]; then rm -Rf build; fi
if [ -d "bin" ]; then rm -Rf bin; fi

# Create build directory
mkdir -p build
mkdir -p bin

cd build

# Generate build files
cmake ../

# Build project
cmake --build .

# Exit build directory
cd ..

# Move executable out
mv build/cpp_satellite_analyzer_project bin

# Destroy build directory
rm -r build

echo Done!
