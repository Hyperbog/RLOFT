@echo off
setlocal
rem Opt-in forensic session. Blocks until the executable exits, then ZIPs
rem one bounded Level22 trace. Never modifies the original game files.
set "ROOT=%TEMP%\RLOFT-Demo-Diagnostics"
if not exist "%~dp0ravenloft_demo_v1.exe" (
  echo Ravenloft executable not found next to Diagnose-Level22.cmd.
  pause
  exit /b 1
)
if exist "%ROOT%\latest-session.txt" del /q "%ROOT%\latest-session.txt"
echo LEVEL 22 DIAGNOSTICS
echo Press F12 while the game is open to mark a defect.
echo Close/exit the game to package the report automatically.
echo.
"%~dp0ravenloft_demo_v1.exe" --explore-level22 --dev-inspect --dev-interact --dev-trace %*
set "GAME_STATUS=%ERRORLEVEL%"
if not exist "%ROOT%\latest-session.txt" (
  echo No diagnostic session was created. Game exit code: %GAME_STATUS%
  pause
  exit /b 2
)
set /p "RLOFT_SESSION=" < "%ROOT%\latest-session.txt"
if not defined RLOFT_SESSION (
  echo Diagnostic session path was unavailable.
  pause
  exit /b 3
)
if not exist "%RLOFT_SESSION%\manifest.json" (
  echo Diagnostic session is incomplete: "%RLOFT_SESSION%"
  pause
  exit /b 4
)
powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -LiteralPath $env:RLOFT_SESSION -DestinationPath ($env:RLOFT_SESSION + '.zip') -Force -ErrorAction Stop"
if errorlevel 1 (
  echo ZIP packaging failed. The original diagnostic folder is preserved:
  echo %RLOFT_SESSION%
  pause
  exit /b 5
)
echo.
echo Diagnostic ZIP ready to attach to the project conversation:
echo %RLOFT_SESSION%.zip
echo.
echo Game exit code: %GAME_STATUS%
pause
endlocal
