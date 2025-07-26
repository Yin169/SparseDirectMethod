#!/bin/bash

set -e  # Exit immediately if a command fails

# Build script for SparseDirectMethods project

echo "Building SparseDirectMethods project..."

# Remove build directory if it exists
if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi

# Create build directory
mkdir -p build

# Change to build directory
cd build || { echo "Failed to enter build directory"; exit 1; }

# Run CMake
echo "Running CMake..."
cmake ..

# Build the project with clean first
echo "Cleaning previous build..."
make clean || echo "No previous build to clean"

echo "Building project..."
make

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Run the executable with: ./build/sparse_direct_methods"
else
    echo "Build failed!"
    exit 1
fi