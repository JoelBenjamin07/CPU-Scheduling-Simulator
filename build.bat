@echo off
setlocal
pushd "%~dp0"

where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: g++ was not found.
  echo Install MinGW-w64 or MSYS2 MinGW-w64, and add its bin folder to PATH.
  echo Then close and reopen VS Code.
  popd
  exit /b 1
)

if not exist build mkdir build
if errorlevel 1 (
  echo ERROR: Could not create the build directory.
  popd
  exit /b 1
)

echo [1/2] Building KernelSim application...
g++ -std=c++17 -Wall -Wextra -pedantic -g -Isrc src\Process.cpp src\SchedulingPolicy.cpp src\SchedulingEngine.cpp src\ResourceManager.cpp src\FileManager.cpp src\ConsoleUI.cpp src\main.cpp -o build\KernelSim.exe
if errorlevel 1 (
  echo ERROR: Application build failed. See compiler messages above.
  popd
  exit /b 1
)

echo [2/2] Building KernelSim tests...
g++ -std=c++17 -Wall -Wextra -pedantic -g -Isrc src\Process.cpp src\SchedulingPolicy.cpp src\SchedulingEngine.cpp src\ResourceManager.cpp src\FileManager.cpp tests\TestMain.cpp -o build\KernelSimTests.exe
if errorlevel 1 (
  echo ERROR: Test build failed. See compiler messages above.
  popd
  exit /b 1
)

echo.
echo BUILD SUCCESSFUL
 echo Application: %CD%\build\KernelSim.exe
echo Tests:       %CD%\build\KernelSimTests.exe
echo To launch the app, run run.bat or press F5 in VS Code.
popd
exit /b 0
