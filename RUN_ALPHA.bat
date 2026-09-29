@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"
set "BUILD_DIR=build-alpha"
set "ALPHA_EXE="
for /r "%BUILD_DIR%" %%F in (RavenloftAlpha.exe) do (
    if exist "%%~fF" set "ALPHA_EXE=%%~fF"
)
if not defined ALPHA_EXE (
    echo RavenloftAlpha.exe has not been built yet.
    echo Running BUILD_AND_RUN_ALPHA.bat now...
    call BUILD_AND_RUN_ALPHA.bat
    exit /b %errorlevel%
)
start "Ravenloft Port Alpha" "!ALPHA_EXE!"
