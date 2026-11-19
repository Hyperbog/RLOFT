@echo off
setlocal EnableExtensions DisableDelayedExpansion
title Ravenloft Stone Prophet - Original DOS startup diagnostics
cd /d "%~dp0"
echo.
echo =============================================================
echo STONE PROPHET - ORIGINAL DOS RUNTIME CAPTURE
echo =============================================================
echo.
echo Use THIS launcher, not a double-click on dosbox-x.exe.
echo This window will remain open and preserve any startup error.
echo.
if not exist "%~dp0dosbox-x.exe" (
  echo ERROR: dosbox-x.exe must be in the same folder as this launcher.
  goto :end
)
if not exist "%~dp0Start-Stone-Capture.ps1" (
  echo ERROR: Start-Stone-Capture.ps1 is missing from this folder.
  goto :end
)
echo Enter the ORIGINAL installed PROPHET folder, containing SP.EXE.
echo Example: C:\Games\Stone Prophet\PROPHET
set /p "STONE_GAME=PROPHET directory: "
echo.
echo Enter the ORIGINAL extracted CD_Extracted folder, containing LEVELS.DAT.
set /p "STONE_CD=CD_Extracted directory: "
set "STONE_GAME=%STONE_GAME:"=%"
set "STONE_CD=%STONE_CD:"=%"
if not exist "%STONE_GAME%\CODE.2" (
  echo ERROR: CODE.2 is not in the provided PROPHET folder.
  goto :end
)
if not exist "%STONE_CD%\LEVELS.DAT" (
  echo ERROR: LEVELS.DAT is not in the provided CD folder.
  goto :end
)
set "DIAGLOG=%TEMP%\Stone-Original-Startup-%RANDOM%-%RANDOM%.log"
echo.
echo Capturing PowerShell startup messages in:
echo %DIAGLOG%
echo.
echo DOSBox will remain at its guest C: prompt if SP.EXE exits.
echo Read any DOS error BEFORE typing EXIT in that window.
echo.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Start-Stone-Capture.ps1" -GameDir "%STONE_GAME%" -CdDir "%STONE_CD%" -DosboxExe "%~dp0dosbox-x.exe" >"%DIAGLOG%" 2>&1
set "STATUS=%ERRORLEVEL%"
echo.
echo ----------------- STARTUP OUTPUT -----------------
type "%DIAGLOG%"
echo --------------------------------------------------
echo.
echo Windows launcher exit code: %STATUS%
echo Startup log saved at: %DIAGLOG%
echo For further diagnosis, send the exact error shown above or in DOSBox.
:end
echo.
pause
endlocal
