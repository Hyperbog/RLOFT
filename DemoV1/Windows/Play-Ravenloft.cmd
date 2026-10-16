@echo off
setlocal EnableExtensions DisableDelayedExpansion
title Ravenloft - Enhanced Game
if not exist "%~dp0Play-Ravenloft.ps1" (
  echo Missing paired Play-Ravenloft.ps1 script.
  pause
  exit /b 1
)
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Play-Ravenloft.ps1"
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" (
  echo.
  echo Alpha ended with exit code %RESULT%.
  echo Your original Game folder was not modified.
  pause
)
exit /b %RESULT%
