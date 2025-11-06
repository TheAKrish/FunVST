#!/bin/bash

# SimpleSampler Build Script for macOS
# This script automates the build process

set -e  # Exit on error

echo "=================================="
echo "SimpleSampler Build Script"
echo "=================================="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found!"
    echo "Please run this script from the SimpleSampler directory."
    exit 1
fi

# Check if JUCE exists
if [ ! -d "../JUCE" ]; then
    echo "Error: JUCE framework not found!"
    echo "Expected location: ../JUCE"
    echo "Please ensure JUCE is in the parent directory."
    exit 1
fi

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed!"
    echo "Install with: brew install cmake"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo ""
echo "Configuring project with CMake..."
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS - use Xcode generator
    cmake .. -G Xcode
else
    # Linux/Windows - use default generator
    cmake .. -DCMAKE_BUILD_TYPE=Release
fi

# Build the project
echo ""
echo "Building SimpleSampler..."
cmake --build . --config Release

# Check if build was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "=================================="
    echo "Build completed successfully!"
    echo "=================================="
    echo ""

    if [[ "$OSTYPE" == "darwin"* ]]; then
        echo "Plugin locations:"
        echo "  VST3: ~/Library/Audio/Plug-Ins/VST3/SimpleSampler.vst3"
        echo "  AU:   ~/Library/Audio/Plug-Ins/Components/SimpleSampler.component"
        echo ""
        echo "To validate AU plugin, run:"
        echo "  auval -v aumu Ssmp Ynam"
    else
        echo "Plugin built in: build/SimpleSampler_artefacts/Release/"
    fi

    echo ""
    echo "Next steps:"
    echo "1. Open your DAW (Logic Pro, Ableton, Reaper, etc.)"
    echo "2. Rescan plugins if needed"
    echo "3. Create a new instrument track"
    echo "4. Load SimpleSampler"
    echo "5. Click 'Load Sample' to load a WAV file"
    echo "6. Play the virtual keyboard!"
    echo ""
else
    echo ""
    echo "Build failed! Check the errors above."
    exit 1
fi
