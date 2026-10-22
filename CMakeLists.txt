cmake_minimum_required(VERSION 3.15)
project(KernelSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(KERNELSIM_CORE
    src/Process.cpp
    src/SchedulingPolicy.cpp
    src/SchedulingEngine.cpp
    src/ResourceManager.cpp
    src/FileManager.cpp
)

add_executable(KernelSim
    ${KERNELSIM_CORE}
    src/ConsoleUI.cpp
    src/main.cpp
)

target_include_directories(KernelSim PRIVATE src)

add_executable(KernelSimTests
    ${KERNELSIM_CORE}
    tests/TestMain.cpp
)

target_include_directories(KernelSimTests PRIVATE src)

enable_testing()
add_test(NAME KernelSimTests COMMAND KernelSimTests)
