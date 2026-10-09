@echo off
setlocal
rem Original RES4 resource-candidate audit, NOT the DOS game renderer.
rem Uses the player's own original GOG/Steam or loose installation.
rem Any optional quoted argument overrides automatic game discovery.
set "TOOL=%~dp0rloft_res4_ground_probe.exe"
if not exist "%TOOL%" (
  echo Missing rloft_res4_ground_probe.exe from Windows build artifact.
  pause
  exit /b 1
)
set "REPORT=%TEMP%\RLOFT-Level22-RES4-Ground-%RANDOM%.txt"
"%TOOL%" %* >"%REPORT%" 2>&1
set "RESULT=%ERRORLEVEL%"
type "%REPORT%"
echo.
echo Report: "%REPORT%"
echo.
echo This report resolves the actual Level22 ground texture using
echo original LEVELS.DAT header+7 and original INIT+0x0E.
echo It also enumerates other 64x64 candidates for source auditing.
echo It does not modify the rendered demo or any original game data.
echo.
pause
exit /b %RESULT%
