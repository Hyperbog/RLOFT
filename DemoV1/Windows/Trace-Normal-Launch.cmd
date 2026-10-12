@echo off
setlocal EnableExtensions DisableDelayedExpansion
rem M-PLAY-01 incomplete NATIVE reconstruction, NOT original R.EXE.
rem This tool uses owned original data but DOES NOT execute the DOS game.
rem No Level 22 warp, injected party or developer interaction overrides.
rem Normal native startup MAY write CHARS/SAVE. Use an isolated copy.
set "ROOT=%TEMP%\RLOFT-Demo-Diagnostics"
echo RAVENLOFT NATIVE REIMPLEMENTATION - INCOMPLETE FIELD TEST
echo This is NOT the original DOS game and does NOT execute R.EXE.
echo It loads selected original game DATA into the partial native engine.
echo You will choose the exact original installation in a folder dialog.
echo The native engine currently begins at Level 22 with existing CHARS.
echo For a cold start, test a separate copy without CHARS.
echo Use an isolated installation/runtime copy. Back up CHARS and SAVE.DAT.
echo The native engine may write files during normal play.
echo F12 marks an exploration frame. Exit to package a report.
echo.
if not exist "%~dp0ravenloft_demo_v1.exe" (
  echo Native executable missing beside Trace-Normal-Launch.cmd.
  pause
  exit /b 1
)
if exist "%ROOT%\latest-session.txt" del /q "%ROOT%\latest-session.txt"
rem Force an explicit folder chooser, preventing a silently cached install.
rem Do not forward arbitrary options or enable the old preview warp.
"%~dp0ravenloft_demo_v1.exe" --dev-trace --choose-install
set "GAME_STATUS=%ERRORLEVEL%"
if not exist "%ROOT%\latest-session.txt" (
  echo No exploration trace was created. The native engine may have stopped
  echo before exploration or the original-media chooser was cancelled.
  echo Record a screenshot and
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
