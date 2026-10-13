@echo off
setlocal EnableExtensions DisableDelayedExpansion
title Ravenloft - Alpha Normal-Game Field Test
echo.
echo ALPHA FIELD TEST - incomplete native original-game reconstruction.
echo The original DOS executable is NOT being executed.
echo The original game data you select is copied to a disposable
echo local workspace BEFORE the native game is launched.
echo CHARS and SAVE.DAT in your selected original folder are not changed.
echo No game media or diagnostic report is uploaded automatically.
echo.
if not exist "%~dp0ravenloft_demo_v1.exe" (
  echo Missing ravenloft_demo_v1.exe next to this launcher.
  pause
  exit /b 1
)
if not exist "%~dp0Test-Alpha-Continuous-Game.ps1" (
  echo Missing paired Alpha field-test PowerShell script.
  pause
  exit /b 1
)
powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -File "%~dp0Test-Alpha-Continuous-Game.ps1"
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" (
  echo Alpha field test ended with exit code %RESULT%.
)
echo.
echo Keep the printed session ZIP, copied test installation, and
echo screenshot of the first reproducible blocker for review.
pause
exit /b %RESULT%
