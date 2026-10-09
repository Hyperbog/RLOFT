echo off
cls
echo %0
pause
if "%0.bat" == "install.bat" goto fine
echo Please change to the directory that contains the
echo Curse of the Azure Bonds Program and re-run install.
echo Note: type "install" in all lower-case letters.
goto exit

:fine
if "%1" == "" goto nodrive
if "%2" == "" goto nodrive
echo Installing as %1:\%2
md %1:\%2
xcopy install.bat %1:\%2 /s
if not exist %1:\%2\install.bat goto died
xcopy *.* %1:\%2 >nul /s
if exist walldef6.dax goto cdone

:nodrive
echo Usage: install X Y
echo where X=letter of your hard disk.
echo where Y=name of the directory
echo Example: install c CURSE
goto exit

:died
echo There was an error creating %1:\%2
echo Curse of the Azure Bonds was not installed.
goto exit

:wrongo
echo Install must be run from The Curse of the Azure Bonds Cd only.
goto exit

:cdone
cd %1:\%2
%1:

cls
echo Curse of the Azure Bonds is now installed.
echo Run START.EXE to start playing Curse of the Azure Bonds.
:exit

