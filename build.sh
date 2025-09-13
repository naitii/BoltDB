.\h#!/bin/bash

echo "Building BoltDB..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . --config Release

echo ""
echo "Build complete! Executable is in build/bin/boltdb"
echo ""
echo "To run the server:"
echo "  ./build/bin/boltdb"
echo ""
echo "To run the test client:"
echo "  g++ -std=c++17 -pthread -o test_client ../test_client.cpp"
echo "  ./test_client"
