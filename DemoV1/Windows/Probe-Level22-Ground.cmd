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
echo This report enumerates 64x64 source-backed RES4 texture CANDIDATES.
echo It does NOT assert which one the original DOS runtime selected.
echo.
pause
exit /b %RESULT%
