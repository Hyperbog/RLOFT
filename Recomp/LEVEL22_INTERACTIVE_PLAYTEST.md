# Original CODE.2 Level 22 — first interactive Windows playtest

The native original-source player host already exists at `Recomp/src/original_code2_level22_window.cpp`, is compiled by the Windows CMake build, and is packaged by the Windows milestone workflow. This uses reconstructed original CODE.2 (not the enhanced demo engine).

## Launch

1. Download the package from the [latest successful Windows milestone](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-milestone-windows.yml) under **Artifacts**. Extract **the complete package**.
2. Double-click `Play-RLOFT-Level22.cmd`. The launcher finds standard GOG installation directories or asks you to choose the original game `RLOFT` folder containing `CODE.2`.
3. Supply your own unmodified GOG installation, including `game.gog` in the parent directory. A custom extracted CD resource folder can be passed using `-CdRoot`.

Controls: **W/S** move forward/back, **A/D** strafe, **J/L** or **left/right arrow keys** turn, **Esc** quit. The native host shows the live original 320×400 indexed-VGA scanout and reports source position/heading/movement counters.

Example for custom paths or non-graphical smoke check:

```powershell
powershell -ExecutionPolicy Bypass -File .\Launch-RLOFT-Level22.ps1 -GameFolder 'C:\Games\RLOFT'
powershell -ExecutionPolicy Bypass -File .\Launch-RLOFT-Level22.ps1 -GameFolder 'C:\Games\RLOFT' -CdRoot 'C:\Games\CD_Extracted\RLOFT' -Smoke
```

The original game files and saves are read-only. Original CD data extracted to a private temp directory are cleaned up on exit; none is published in the compiled package.

## Acceptance limits

The preserved-media Windows source tests verify the original keyboard loop, 16 keyboard polls, 9 actual source camera displacements, and 7 heading rotations. The source native-window smoke exercises seven inputs without opening the GUI, but **a human GUI playtest has not yet been certified**. Some game events intentionally stop when source callbacks are not connected; independent retail DOS indexed framebuffer comparison, physical DOS IRQ behavior, audio, full NPC interactions and cross-level gameplay remain open. This is a first Level22 experimental exploration build, **not a complete game**.
