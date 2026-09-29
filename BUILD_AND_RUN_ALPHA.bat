@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"

echo ============================================================
echo   Ravenloft Port - Alpha Build and Run
echo ============================================================
echo.

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: CMake was not found in PATH.
    echo Install Visual Studio C++ Desktop Development with CMake tools,
    echo then run this file again.
    echo.
    pause
    exit /b 1
)

set "BUILD_DIR=build-alpha"

if not exist "%BUILD_DIR%\CMakeCache.txt" (
    echo [1/3] Configuring the Windows alpha...
    cmake -S . -B "%BUILD_DIR%"
    if errorlevel 1 goto :fail
) else (
    echo [1/3] Existing alpha build configuration found.
)

echo [2/3] Building RavenloftAlpha in Release mode...
cmake --build "%BUILD_DIR%" --config Release --target RavenloftAlpha
if errorlevel 1 goto :fail

set "ALPHA_EXE="
for /r "%BUILD_DIR%" %%F in (RavenloftAlpha.exe) do (
    if exist "%%~fF" set "ALPHA_EXE=%%~fF"
)

if not defined ALPHA_EXE (
    echo ERROR: Build succeeded but RavenloftAlpha.exe could not be located.
    goto :fail
)

echo [3/3] Launching:
echo       !ALPHA_EXE!
echo.
start "Ravenloft Port Alpha" "!ALPHA_EXE!"
exit /b 0

:fail
echo.
echo ============================================================
echo BUILD FAILED.
echo Copy the error text above into ChatGPT so it can be diagnosed.
echo ============================================================
echo.
pause
exit /b 1
