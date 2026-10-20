@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -STA -File "%~dp0Launch-Ravenloft-DAC.ps1" -CompatibilityMode
if errorlevel 1 (
  echo.
  echo Diagnostic launcher failed; the PowerShell error should be visible above.
  pause
)
