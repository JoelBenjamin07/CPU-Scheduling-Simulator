# Build and Demonstration Checklist

## On your development machine

- [ ] Install MinGW-w64 with a C++17-capable `g++`, or install a recent CMake and compiler.
- [ ] Extract the `KernelSim` folder.
- [ ] Open the terminal at the folder containing `CMakeLists.txt`.
- [ ] Run `build.bat` on Windows or `./build.sh` in a GCC shell.
- [ ] Run `build/KernelSimTests` or `build\\KernelSimTests.exe`.
- [ ] Confirm the test output says `All 13 KernelSim tests passed.`
- [ ] Run the application from the project root.
- [ ] Confirm both files under `data/` are present.
- [ ] Test one scheduling algorithm, algorithm comparison, Banker's safety, and an unsafe request.
- [ ] Export one report and confirm `results/last_report.txt` is created.

## Before submitting

- [ ] Replace no source code with code you cannot explain.
- [ ] Ensure the project report matches the final implementation.
- [ ] Include source files, build scripts, sample data, tests, and report.
- [ ] Keep the project terminal-oriented; do not add a rushed GUI.
- [ ] Rehearse the six-minute demonstration.
- [ ] Be able to explain the metric formulas, SJF vs SRTF, Round Robin quantum, safe state, and rollback.
- [ ] Keep a backup copy of the project ZIP.

## Running on another Windows machine

The recipient needs a C++17-capable compiler to build from source. The project does not bundle MinGW or any third-party runtime. Install MinGW-w64, add its `bin` directory to `PATH`, then run `build.bat` from the extracted project root.
