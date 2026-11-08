# Part 33: Native original CODE.2 Level22 window (2026-10-25)

The first Windows experimental slice presents source-authenticated original VGA and responds to Windows key events through the retained CODE.2 source execution. No original commercial game files or captures are shipped.

## Source path

Original DOS CODE.2 source is executed on the same retained guest and 11 original RES archives through 353A0, 20440, 16D04, 181B4 and 1FC14. A Win32 GDI window uses OriginalVgaHost scanout indices at 320x400 and original source DAC RGB6 palette. It does not draw host-invented dungeon art.

W/S move forward/back, A/D strafe, J/L or left/right arrows turn, Escape closes. A 120 ms host timer samples held keys and submits one original 30B40 scan code per source poll. Camera and heading are read from guest RAM after compiled 2E120 and 2DBAC, displayed with actual displacement/rotation counters. Unconnected original events fail closed rather than fabricate completion. The experimental WinMM audio path remains opt-in and is NOT enabled automatically.

## Run owner-local

Download the most recent successful Recomp Windows milestone engineering ZIP. Extract all files together and double-click Play-RLOFT-Level22.cmd. Defaults to original installation at D:\GOG Games\Ravenloft - Strahd's Possession\RLOFT with game.gog alongside the RLOFT folder. To override, invoke Launch-RLOFT-Level22.ps1 with -GameFolder path. Optionally provide -CdRoot path to an already-extracted original CD RLOFT folder.

CD resources are unpacked locally and removed on exit. No original saves are overwritten.

## Acceptance limits

This is not complete retail gameplay. CI smoke asserts seven guest source polls, an original 320x400 indexed VGA scanout, and camera/heading motion; it does not validate graphical accuracy of the Windows window on a user's machine. DOS keyboard IRQ9 timing, synchronized DOS indexed framebuffer parity, all scripted interactions, combat, cross-level transitions, sound devices, and normal saves remain incomplete. Original CODE.2 guest remains authoritative for all dungeon movement and picture content.
