@echo off
setlocal
call "%~dp0build.bat"
if errorlevel 1 exit /b 1
pushd "%~dp0"
echo.
echo Running KernelSim automated tests...
"%~dp0build\KernelSimTests.exe"
set TEST_EXIT=%ERRORLEVEL%
popd
if not "%TEST_EXIT%"=="0" (
  echo TESTS FAILED with exit code %TEST_EXIT%.
  exit /b %TEST_EXIT%
)
echo TESTS PASSED.
exit /b 0
