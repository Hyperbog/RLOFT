# Ravenloft enhanced game — continuous product consolidation

**Date:** 2026-10-15 · **Canonical game branch:** `demo-v1` (historical
directory name retained; **not** a disposable demo) · **Release:** Pre-Alpha.

## What the developer builds actually contain

The original owner-tested enhanced viewport was independently developed in
`native-engine`. Relevant source files are preserved at
`NativeEngine/src/win32_native_gpu_exploration.cpp`,
`native_gpu_world_mesh.cpp`, `native_win32_gl_target.cpp`,
`native_scene_instances.cpp`, `native_world_payload.cpp`,
`native_type7_scene_shape.cpp`, and `native_source_scene_compositor.cpp`
on that branch. Its Win32 OpenGL prototype supports 3D free mouse yaw/pitch,
original RES4 mipmapped texture filtering, source-world geometry, variable
internal render resolution, fullscreen, 2×/4× AA, running and an explored
automap. Original `SINTAB` and the dumped machine code were used as research
inputs there. **That history has not been deleted.**

The current complete startup / character-creation / native CODE.2 gameplay
and inventory work is in `demo-v1`, chiefly
`DemoV1/src/win32_main.cpp`, `exploration_runtime.cpp`,
`exploration_presenter.cpp`, and the original RES* decoders.
The world currently renders into a faithful 320×400 indexed software
framebuffer presented by Win32 GDI. Previous `demo-v1` releases therefore
looked unlike the 3D GPU prototype, despite passing gameplay-focused tests.

**A separate GPU explorer is NOT a replacement for the continuous game.**
The missing task is to move its GPU presentation and world-geometry model into
the running game and wire it to the same live character/world/inventory/event
state, not reintroduce a launcher that warps to Level 22.

## Changes committed to the actual game in this consolidation

1. Removed the standalone `Explore-Level22.cmd` walkaround launcher.
   Developer-only capture/source replay paths remain for debugging but are not
   presented as an alternative player build.
2. Replaced three overlapping Windows player downloads with one
   `Ravenloft-Enhanced-Game-Windows-x64` artifact, containing
   `Ravenloft.exe`, `Play-Ravenloft.cmd`, its safe `Play-Ravenloft.ps1`
   backing script, original-media folder instructions, executable hash and
   exact compiled-source stamp. The underlying CMake target and checked-in
   Windows binary retain old internal filenames only to avoid breaking
   development history.
3. Removed **"Demo Build v1"** from the actual game window title.
4. Integrated WASD movement, Q/E turning and A/D strafing into the live
   keyboard/locomotion path. Arrow/numpad controls still work. The original
   bounded 2D collision and movement physics remain authoritative.
5. Shift-to-run halves the player's motion integration interval rather
   than fast-forwarding the whole 60 Hz rendering/music/world loop. A
   synthetic timing regression tests sprint start/stop, missed ticks and
   reset semantics.
6. M (or Ctrl+right-click) toggles captured horizontal mouse yaw. Mouse
   rotation is sampled at 60 Hz independently of the lower-frequency
   movement/collision integration. Normal right-click inventory/hand
   interactions are preserved. Input/capture is cleared on focus loss,
   automap, inventory and non-viewport modes. **Vertical GPU pitch is not
   yet integrated; do not advertise this as full GPU free-look.**
7. Tab opens/closes the existing game-world automap, using the same
   authored controller and cache used by its menu button. Modal key events
   never leak back into movement. Synthetic UI tests cover repeat behavior.
8. F3 toggles smooth/nearest scaling of the **final indexed PC game
   framebuffer**; F11 toggles fullscreen with restoration of the prior
   Windows placement. **F3 is not true texture upscaling/mipmapping or
   a substitute for importing the original OpenGL world renderer.**

The regression suite still protects the genuine CODE.1 intro, cinematic
sound, animations, character creation, native RES3/RES4/RES8 content,
typed inventory and original-level story state.

## Exact Windows build and package verification

[Windows Actions **#901**](https://github.com/Hyperbog/RLOFT/actions/runs/38828727846)
completed successfully on source
`06f2ce565e3bd1c55a4a6c8f494046fd10f576a2`.

- Windows MSVC compiled the continuous enhanced-game Win32 host and all
  39 focused regression targets; **39/39 CTests passed**. That includes the
  new `exploration_ui_tests` and updated
  `exploration_gameplay_cadence_tests`, plus Level 22 surface and asset
  regressions.
- Launcher/source preflight passed, including **15** negative continuous
  launcher mutations, **17** native scene mutations and **9** in-game UI
  mutations.
- Published exactly **one** player package:
  `Ravenloft-Enhanced-Game-Windows-x64`, artifact
  [12015779493](https://github.com/Hyperbog/RLOFT/actions/runs/38828727846/artifacts/12015779493).
- Independently tested ZIP integrity and verified the packaged
  `Ravenloft.exe` against its `Ravenloft.exe.sha256` manifest.
  SHA-256: `98b538d47e50a1da80c0346d8f617ea9f38a5a920d69514359bee436a3fad490`.
  `build-source-sha.txt` in the ZIP names `06f2ce565e3bd1c55a4a6c8f494046fd10f576a2`.
- Original copyrighted game assets are NOT included; the safe launcher
  discovers/copies owner-supplied media and does not overwrite originals.
  Preserved GPU prototype, original machine-code dumps, standalone source
  diagnostics and all research documentation are still available in
  historical branches.

**What these tests do NOT prove:** correctly textured GPU world scenes,
true 3D free-look/pitch, screenshot-accurate Level 22 tree positions or
working end-to-end game content. The ZIP still uses the 320×400 indexed
software world scanout, not the separate OpenGL world's geometry or
FBO/mipmap pipeline. **Gameplay field/owner visual acceptance remains
open.**

## What is still missing — binding contract for GPU consolidation

**Next code work, in dependency order:**

1. Introduce a portable `GpuWorldFrame` data transfer under the canonical
   game, populated solely from `ExplorationRuntimeFrame`,
   `ExplorationSessionState` and the real current `LevelWorldState`.
   It must not create a second party, duplicate the world or synthesize
   Level 22 coordinates.
2. Port the tested OpenGL context/framebuffer, original RES4 texture upload,
   real world mesh/visibility and RES3 billboard placement from
   `NativeEngine` into a renderer class belonging to the existing game.
   Draw that renderer behind the current party HUD/menus. Test source media
   revision, cache invalidation and life-cycle on level transitions.
3. Make original source camera heading/position and modern pitch/FOV values
   separate and explicit so mouse look never silently changes collision,
   script triggers or persisted player heading. Map arrows/QE/M uniformly.
4. Bind game-world NPC deaths, loose-item pickup/drop, scene object state,
   inventory equipment and animated resource selector to the SAME renderer.
   A static reconstructed world screenshot is not game integration.
5. Retire the GDI world scanout only after real field comparisons show that
   the GPU view renders actual Level 22 trees, assassin, ground, walls and
   all gameplay chrome without flicker or audio/animation regression.
6. Extend save/restore to the full mutable world; complete scripted menu
   options and authored campaign. Quicksave-only / isolated explorer
   success cannot pass Beta gates.

**Acceptance must include:** single normal player launcher, original intro
and creation, live Level 22, moving/turning/strafing/running in the same
runtime, GPU yaw+pitch, source-real texture filtering, automap and encounter,
background audio, repeatable save/load and error-free replay. Screenshots,
video and owner test remain required; CI alone is not visual acceptance.

No original game assets or machine-code dumps are published in an enhanced
game artifact. `DnD-Classics`, `native-engine`, and reverse-engineering
branches remain preserved as research provenance.
