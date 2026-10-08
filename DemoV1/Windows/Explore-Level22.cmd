@echo off
setlocal
rem Native-only environment walkaround, not a saved gameplay character.
rem Place this launcher next to ravenloft_demo_v1.exe. Any optional quoted
rem installation directory is forwarded to the existing asset locator.
if not exist "%~dp0ravenloft_demo_v1.exe" (
  echo Ravenloft Demo v1 executable not found next to this launcher.
  echo Build the demo or obtain the Windows build artifact first.
  pause
  exit /b 1
)
start "" "%~dp0ravenloft_demo_v1.exe" --explore-level22 %*
endlocal
