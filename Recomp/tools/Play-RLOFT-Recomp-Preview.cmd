@echo off
setlocal
cd /d "%~dp0"
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -STA -File "%~dp0Launch-RLOFT-Recomp-Preview.ps1"
set "RESULT=%ERRORLEVEL%"
echo.
echo Preview finished. Read the output folder under Documents\RLOFT-Recomp-Preview.
if not "%RESULT%"=="0" echo Launcher ended with error code %RESULT%.
pause
exit /b %RESULT%
