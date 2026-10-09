@echo off
setlocal
rem Original CODE.1 class x race starting loadouts, no saved game used.
set "TOOL=%~dp0rloft_starting_equipment_probe.exe"
if not exist "%TOOL%" (
  echo Missing rloft_starting_equipment_probe.exe in this Windows build.
  pause
  exit /b 1
)
set "REPORT=%TEMP%\RLOFT-Original-Starting-Equipment-%RANDOM%.txt"
"%TOOL%" %* >"%REPORT%" 2>&1
set "RESULT=%ERRORLEVEL%"
type "%REPORT%"
echo.
echo Original starting equipment report: "%REPORT%"
echo Each class and race combination is taken from CODE.1.
echo Item names are taken directly from OBJECTS.DAT.
echo No game files or characters were modified.
pause
exit /b %RESULT%
