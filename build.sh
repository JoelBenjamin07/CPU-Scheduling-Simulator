#!/usr/bin/env sh
set -eu
mkdir -p build
g++ -std=c++17 -Wall -Wextra -pedantic src/Process.cpp src/SchedulingPolicy.cpp src/SchedulingEngine.cpp src/ResourceManager.cpp src/FileManager.cpp src/ConsoleUI.cpp src/main.cpp -o build/KernelSim
g++ -std=c++17 -Wall -Wextra -pedantic src/Process.cpp src/SchedulingPolicy.cpp src/SchedulingEngine.cpp src/ResourceManager.cpp src/FileManager.cpp tests/TestMain.cpp -o build/KernelSimTests
printf 'Build completed.\nRun ./build/KernelSim\nRun ./build/KernelSimTests to execute tests.\n'
