@echo off
setlocal
pushd "%~dp0"
if not exist build\KernelSim.exe (
  echo KernelSim.exe was not found. Building the project first...
  call build.bat
  if errorlevel 1 (
    popd
    exit /b 1
  )
)
"%~dp0build\KernelSim.exe"
set APP_EXIT=%ERRORLEVEL%
popd
exit /b %APP_EXIT%
