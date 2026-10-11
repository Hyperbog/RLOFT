RAVENLOFT NATIVE ENGINE - SOURCE-SHARED LIVE WINDOWS PREVIEW
==========================================================

This is an EXPERIMENTAL independent Windows x64 executable built from the
same reconstructed original game-core source and existing decomp demo's
working live cinematic/menu/character-creation/exploration controllers.

Executable:
  rloft_native.exe

It is NOT a renamed or copied DemoV1 executable. NativeEngine has a
separate Win32 entrypoint, independent CMake product target, registry
identity, and user profile. It reuses the original-source-traced
live game code rather than duplicating cinematic/creation logic.

Original game assets are NOT included. Run rloft_native.exe on Windows
with an original DOS/GOG/Steam installation. GameSource tries existing
install discovery; you may supply the install folder path as its one
positional argument. Do NOT use any developer-only flags.

NativeEngine writable state is isolated to:
  %LOCALAPPDATA%\Hyperbog\RLOFT\NativeEngine\Default\

Existing original CHARS, SAVE.DAT, GAME.OPT and other mutable files in
the original game folder are NOT imported automatically and are NEVER
used as the native profile's writable path. New CODE.1-created CHARS
files are written into the native profile, not the original install.
The source install should be treated as read-only.

Normal cold startup uses existing DemoV1 original reconstructed behavior:
CINE00/CINE12, original palette/timed logos/menu, original selection,
CODE.1 character creation, transfer to CODE.2 and attempted world entry.
The built executable is an early integration proof; full installed-game
playability, graphical DOS parity and save compatibility are NOT
independently accepted until user field testing.

Known intentional difference: original existing-party CHARS files are
not automatically imported; they remain in the owner's game folder.
Future explicit import must go through a separate safe profile workflow.

This ZIP is a DEVELOPMENT FIELD-TEST PREVIEW, not a finished game release.
Report the first actual blocker, screenshot and source-revision manifest.

Source project:
  https://github.com/Hyperbog/RLOFT/tree/native-engine/NativeEngine
Integration plan:
  https://github.com/Hyperbog/RLOFT/blob/native-engine/NativeEngine/DEMO_BOOT_PIPELINE_REUSE_20261011.md
