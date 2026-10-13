@echo off
setlocal EnableExtensions DisableDelayedExpansion
title Ravenloft - Continuous Native Alpha
if not exist "%~dp0Play-Ravenloft-Alpha.ps1" (
  echo Missing paired Play-Ravenloft-Alpha.ps1 script.
  pause
  exit /b 1
)
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Play-Ravenloft-Alpha.ps1"
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" (
  echo.
  echo Alpha ended with exit code %RESULT%.
)
echo.
echo The original Game folder was not modified.
echo Press any key to close this diagnostic console.
pause >nul
exit /b %RESULT%
