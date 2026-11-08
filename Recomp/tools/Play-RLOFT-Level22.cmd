@echo off
setlocal
cd /d "%~dp0"
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -STA -File "%~dp0Launch-RLOFT-Level22.ps1"
set "RESULT=%ERRORLEVEL%"
if not "%RESULT%"=="0" (
 echo Level22 native source host stopped. Read the error above.
 pause
)
exit /b %RESULT%
