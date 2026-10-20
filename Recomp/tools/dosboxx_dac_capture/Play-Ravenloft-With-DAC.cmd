@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -STA -File "%~dp0Launch-Ravenloft-DAC.ps1"
if errorlevel 1 (
  echo.
  echo Launcher failed. Read the error above. Check that the folder contains R.EXE and RLOFT.BAT.
  pause
)
