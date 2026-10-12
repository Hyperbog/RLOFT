@echo off
setlocal EnableExtensions DisableDelayedExpansion
rem M-PLAY-01 ordinary startup: NO Level 22 shortcut, injected party,
rem interaction overrides, synthetic state, or original media modifications.
rem The native game itself may write CHARS/SAVE. Use a disposable copy.
set "ROOT=%TEMP%\RLOFT-Demo-Diagnostics"
echo Ravenloft M-PLAY-01: NORMAL LAUNCH plus read-only native trace.
echo Use an isolated installation/runtime copy. Back up CHARS and SAVE.DAT.
echo This launcher does NOT prevent normal original-game state writes.
echo Follow the authentic intro/menu/creation path. No developer shortcut.
echo F12 marks a frame once exploration starts. Exit the game to package it.
echo.
if not exist "%~dp0ravenloft_demo_v1.exe" (
  echo Native executable missing beside Trace-Normal-Launch.cmd.
  pause
  exit /b 1
)
if exist "%ROOT%\latest-session.txt" del /q "%ROOT%\latest-session.txt"
rem Deliberately do not forward arbitrary command-line flags to the game.
rem Owned-media selection uses the ordinary install-discovery/folder UI.
"%~dp0ravenloft_demo_v1.exe" --dev-trace
set "GAME_STATUS=%ERRORLEVEL%"
if not exist "%ROOT%\latest-session.txt" (
  echo No exploration trace was created. The normal game may have stopped
  echo during intro, menus or character creation. Record a screenshot and
  echo the first failed action; do NOT count this as a successful boot.
  echo Native game exit code: %GAME_STATUS%
  pause
  exit /b %GAME_STATUS%
)
set /p "RLOFT_SESSION=" < "%ROOT%\latest-session.txt"
if not defined RLOFT_SESSION (
  echo Invalid trace session pointer.
  pause
  exit /b 3
)
if not exist "%RLOFT_SESSION%\manifest.json" (
  echo Incomplete trace: manifest missing. Preserve diagnostic folder.
  pause
  exit /b 4
)
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -LiteralPath $env:RLOFT_SESSION -DestinationPath ($env:RLOFT_SESSION + '.zip') -Force -ErrorAction Stop"
if errorlevel 1 (
  echo Trace ZIP packaging failed. Session directory preserved.
  pause
  exit /b 5
)
echo.
echo Native trace ZIP: %RLOFT_SESSION%.zip
echo Run audit_mplay01_trace.py on this ZIP using build-source-sha.txt.
echo This trace is NOT DOS equivalence or acceptance of ordinary play.
echo Native game exit code: %GAME_STATUS%
pause
endlocal
