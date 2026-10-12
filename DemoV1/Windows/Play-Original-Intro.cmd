@echo off
setlocal EnableExtensions DisableDelayedExpansion
title Ravenloft - Original Intro and Character Creation
echo Ravenloft: Strahd's Possession - original-game front-end reconstruction
echo.
echo Original cinematic, SSI/DreamForge logo/title, animated menu,
echo and the reconstructed character creation screens.
echo The rest of the original game is NOT YET fully reconstructed.
echo.
echo Choose your owned original game's data folder.
echo This front-end preview never changes CHARS or SAVE.DAT, and
echo never launches the retired Level 22 standalone walkaround.
echo.
if not exist "%~dp0ravenloft_demo_v1.exe" (
  echo Ravenloft native EXE not found alongside the launcher.
  pause
  exit /b 1
)
"%~dp0ravenloft_demo_v1.exe" --front-end-only --choose-install
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" (
  echo Native intro exited with code %RESULT%.
  pause
)
exit /b %RESULT%
