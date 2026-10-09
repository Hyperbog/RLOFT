@echo off
rem ------------------------------------------------------------------------
rem Darksun, The Shattered Lands...
rem
rem This is the audio installer batch file shell to install the TSR's for
rem the General Midi sound cards that require TSR's
rem
rem Author: Ralph Thomas
rem
rem Revisions:  08/16/93 Added the un-installation of the TSR's at end run
rem             08/17/93 Minor revisions...
rem     08-24-93 03:36pm (TP.) revision w/INSTALL.BAT parts
rem ------------------------------------------------------------------------

rem ------------------------------------------------------------------------
rem Detect card
rem ------------------------------------------------------------------------

@echo off

IF "%ULTRADIR%"=="" goto CHECK_ARIA
GOTO GRAVIS

:CHECK_ARIA
IF "%ARIA%"=="" goto NO_TSR
goto ARIA

rem ------------------------------------------------------------------------
rem Detect the Gravis Software Revision and Install the Gravis TSR
rem ------------------------------------------------------------------------

:GRAVIS
IF NOT EXIST %ULTRADIR%\midi\ACPIANO.PAT GOTO OLD_REVISION
rem echo DEBUG: Detecting Software Revision
IF EXIST %ULTRADIR%\midi\HONKY.PAT GOTO V206A
rem echo DEBUG: They have Rev: 2.00
copy UM200.ini ssi1.ini > NUL
rem Copy failed...
IF ERRORLEVEL 1 GOTO G_BADCOPY
rem else Success...
rem echo DEBUG: Rev 2.00 detected and fixed
GOTO G_SUCCESS

:V206A
IF NOT EXIST %ULTRADIR%\midi\TREMSTR.PAT GOTO V206
IF NOT EXIST %ULTRADIR%\midi\CHARANG.PAT GOTO V206
IF NOT EXIST %ULTRADIR%\midi\ECHOVOX.PAT GOTO V206

copy UM206A.INI ssi1.ini > NUL
IF ERRORLEVEL 1 GOTO G_BADCOPY
rem echo DEBUG: Rev 2.06a detected and fixed
pause
goto G_SUCCESS

:V206
copy UM206.INI ssi1.ini > NUL
rem echo DEBUG: Rev 2.06 detected and fixed
pause
IF ERRORLEVEL 1 GOTO G_BADCOPY
goto G_INS_TSR

:G_BADCOPY
cls
echo ษอออออออออออออออออออออออออออออออออออออออออออออออออออออป
echo บ  Gravis UltraMID Installation Error: 1              บ
echo ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออน
echo บ                                                     บ
echo บ  ERROR: Couldn't Copy File Needed for Gravis        บ
echo บ         Native Mode Operation...                    บ
echo บ                                                     บ
echo บ  Another 10k of Disk Space is Required...           บ
echo บ                                                     บ
echo ศอออออออออออออออออออออออออออออออออออออออออออออออออออออผ
pause
GOTO BAD_BOY

:OLD_REVISION
cls
echo ษอออออออออออออออออออออออออออออออออออออออออออออออออออออป
echo บ  Gravis UltraMID Installation Error: 2              บ
echo ฬอออออออออออออออออออออออออออออออออออออออออออออออออออออน
echo บ                                                     บ
echo บ  ERROR: UltraMID TSR is not Supported by Revisions  บ
echo บ         of the Gravis Software Less Then 2.00       บ
echo บ                                                     บ
echo บ         Contact Gravis to Obtain an Update of       บ
echo บ         Their Software...                           บ
echo บ                                                     บ
echo บ          Voice (604) 431-5020                       บ
echo บ          Fax   (604) 431-5155                       บ
echo บ          BBS   (604) 431-5927 V32bis                บ
echo บ          Compuserve: GO PCVENB (71333,350)          บ
echo บ          FidoNET: 1:153/978                         บ
echo บ          InterNET EMail: tech@gravis.com            บ
echo บ          SBCNET: 13:900/3                           บ
echo บ                                                     บ
echo ศอออออออออออออออออออออออออออออออออออออออออออออออออออออผ
pause
GOTO BAD_BOY

:G_INS_TSR
rem Run Gravis TSR
rem lh ultramid -nssi1.ini -q
rem echo DEBUG: Loading the tsr...
lh ultramid -nssi1.ini
IF ERRORLEVEL 1 GOTO BAD_BOY
goto RUN_INSTALLER

:BAD_BOY
set ULTRASND=
set BLASTER=
rem echo DEBUG: Running with No TSR..
pause
goto NO_TSR

:RUN_INSTALLER
cls
sound_ds
rem Uninstall the TSR...
rem ultramid -F -q
ultramid -F
cls
goto END

rem ------------------------------------------------------------------------
rem Install the Aria TSR
rem ------------------------------------------------------------------------

:ARIA
rem Running Aria TSR...
lh miditsr gm2.bnk /I
IF ERRORLEVEL 1 GOTO INS_GM1:

cls
sound_ds
rem Uninstall the TSR...
miditsr /U /I
goto END

:INS_GM1
rem Installation of the GM2 bank failed because of insufficient memory
rem now try the small bank...
lh miditsr gm1.bnk /I
GOTO RUN_ARIA


rem ------------------------------------------------------------------------
rem Run the Sound Installer
rem ------------------------------------------------------------------------


:NO_TSR
cls
sound_ds
goto END

:END
