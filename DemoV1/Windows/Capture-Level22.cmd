@echo off
setlocal
rem Voluntary original-resolution native diagnostic capture.
rem Captures up to 64 changed-cell/facing poses (opt-in), each with
rem .idx (320x400 palette indices), .pal (256x3 raw VGA DAC values),
rem .tsv (C6F4 wall run geometry and immutable CODE.2 mask diagnostics).
rem Captures are in %%TEMP%%\RLOFT-Demo-Captures\<session-id>.
if not exist "%~dp0ravenloft_demo_v1.exe" (
  echo Ravenloft Demo v1 executable not found next to this launcher.
  pause
  exit /b 1
)
echo Captures: %TEMP%\RLOFT-Demo-Captures
start "" "%~dp0ravenloft_demo_v1.exe" --explore-level22 --capture-render --capture-frames=64 %*
endlocal
