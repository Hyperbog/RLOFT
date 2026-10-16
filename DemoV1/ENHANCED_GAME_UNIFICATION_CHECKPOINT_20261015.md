# Ravenloft enhanced game — continuous product consolidation

**Date:** 2026-10-15 · **Canonical game branch:** `demo-v1` (historical
directory name retained; **not** a disposable demo) · **Release:** Pre-Alpha.

## Genuine GPU world integration — source checkpoint (October 15, 2026)

**Source head:** [`736105b1b6b6`](https://github.com/Hyperbog/RLOFT/commit/736105b1b6b6c8bfb2d18bd07d5ef1c0e1340359).
**Windows validation:** [successful run #911](https://github.com/Hyperbog/RLOFT/actions/runs/38832263293), compiled source
`736105b1b6b6c8bfb2d18bd07d5ef1c0e1340359`. Windows MSVC
compilation, launcher validation, **41/41 focused CTests** and **11
deliberate negative GPU source mutations** all passed. Exactly one
`Ravenloft-Enhanced-Game-Windows-x64` artifact was published (ID
`12017453646`); its ZIP contents passed an independent CRC check
and its `Ravenloft.exe` matched the packaged SHA-256:
`b5c3edb79a393fd6ccc0c06daebb62a86b8078a12516be83aaaa04208db3dab5`.
It is a live engine integration and **not** a visual acceptance claim.

The actual CODE.1 → CHARS → CODE.2 game now instantiates
`Win32GpuScene` inside its own Win32 `Host`, at the normal exploration
handoff, rather than opening the old standalone `native-engine` explorer.
`Host::bind_gpu_frame` hands it the current `ExplorationSessionState`,
the same `LevelWorldState` used for NPCs, inventory and encounters, and
the source-authored `world_scene_records` emitted for that frame. No
duplicate party, sandbox world, preview pose or independent exploration
loop can advance behind the player's back.

The GPU mesh was ported from `NativeEngine::build_native_gpu_scene` as
`EnhancedGpuWorldMeshModel::build(const LevelState&)`. It observes the
**mutable current game map**, authentic original 0x2A4-byte resource banks,
special-center dynamic door state, upper wall geometry, floor/ceiling
selectors, outdoor ceiling sentinel and boundary wall sentinel.
An FNV fingerprint prevents rebuilding geometry until cells, source
controls or level identity change. Original RES4 `SurfaceTextureArchive`
decodes source indexed pixels and GLU generates authentic-source-derived
mipmaps without modifying the game files. The original 3D scene is drawn
via batched OpenGL vertex arrays with true viewport aspect ratio.

Per-frame **live** NPC, placed-object and loose-item records select the
original RES3 raster by source ID. Transparent source masks are uploaded
as alpha textures, and the billboards depth-test against original-resourced
GPU walls/floor. Their 0.9-world-unit width and two-unit height are an
**enhanced scene approximation**, NOT authenticated DOS billboard-scale
equivalence. Source placements/identities/selected frame IDs are real;
exact projected size/palette shade need in-game capture validation.

The unchanged 320×400 original indexed framebuffer supplies HUD, party
art, cursor, world backdrop and all modal screens; the GPU draws **only**
in the original first-person viewport. Repainting an active top-edge menu
over 3D prevents a cursor-hover swap back to GDI. Input: W/A/S/D, Q/E,
Shift running, Tab original modal automap, M/Ctrl+right-click to capture
mouse yaw/pitch, F2 GPU/software world toggle, F3 nearest/smooth/mipmap
filter modes and F11 fullscreen. Vertical pitch changes GPU camera only:
the original CODE.2 collision, script state, heading and 16.16 camera
coordinates remain unchanged. GPU failure falls back to the previous
software renderer, not a missing-texture dummy world.

**Source-guard regression:** `verify_game_gpu_integration.py` rejects
11 deliberate deletions of the live game binding, GL textures, sprites,
world resource selectors, menu clipping and player packaging. A separate
`enhanced_gpu_world_mesh_tests` exercises mutable passability, special
door open/closed geometry, outdoor roof sentinel, authored texture banks
and malformed bounds. Compile/CTest status and owner visual acceptance
are independent gates.

**Direct asset sanity check:** The supplied `LEV022.DAT` contains
a 30×15 original grid, authored narrow floor selector 55 and 86
resource-147 boundary cells. The imported mesh deliberately does not
invent drawable wall faces from those no-draw resource entries.
The 110 placed records may include actual trees, but the supplied
RES3 image archive has not been compared to a live GPU capture in this
check. GPU content may still have scale, overlay, shader or animation
defects even if CI is green.

### Further GPU acceptance work

- GPU sky and original RES8 environment should be separated from the
  software sprite/wall background to avoid duplicate fallback artwork
  behind GPU billboards.
- Verify original RES3 tree and assassin scale, animation frame selection,
  alpha/depth layering, palette shading and source-object disappearance.
- Port the previous `NativeGlViewportTarget` FBO resolution presets,
  proper optional 2×/4× MSAA and screenshot capture; current renderer
  renders at the window's actual pixel resolution, not a separate F2
  native-to-4K internal render target.
- Bind level-palette changes and source-texture cache invalidation across
  transitions, preserve complete scripted game/audio state, and field-test
  fullscreen, automap, options, inventory and NPC interaction.
- Do not promote Beta/Final until save/load and full campaign gameplay
  are source-backed, functional and field-accepted.

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
Earlier `demo-v1` releases presented the full 320×400 indexed software
world through Win32 GDI; the new normal-game renderer now layers the
OpenGL 3D scene inside that same original indexed HUD/game window.
The indexed path remains available through F2/fallback.

**A separate GPU explorer is NOT a replacement for the continuous game.**
The GPU presentation, original RES4 geometry and native RES3 billboard
path have now been integrated into the running game using the same live
character/world/inventory records. Field-level visual parity, render
targets/AA and complete authored game progression remain unfinished.

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
   automap, inventory and non-viewport modes. **Vertical GPU camera pitch
   is now available in the live-game OpenGL viewport**, kept separate
   from original gameplay heading and collisions.
7. Tab opens/closes the existing game-world automap, using the same
   authored controller and cache used by its menu button. Modal key events
   never leak back into movement. Synthetic UI tests cover repeat behavior.
8. F3 now cycles GPU original RES4 nearest, smooth and mipmapped
   texture filters; software-only fallback uses nearest/smooth indexed
   presentation. F11 toggles fullscreen. F2 selects GPU world vs
   original software scene, without changing party/AI/encounters.

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
working end-to-end game content. The earlier #901 ZIP still uses the 320×400 indexed
software world scanout; it predates the new integrated OpenGL 3D
geometry/mipmap pipeline. Obtain the newer Windows run once verified. **Gameplay field/owner visual acceptance remains
open.**

## Original GPU renderer follow-on priorities (remaining)

**Next code work, in dependency order:**

1. Complete the GPU backdrop/sprite compositing and animated RES3
   acceptance checks detailed above; source-record binding is implemented,
   but the exact authored billboard scaling and shading remain open.
2. Bring across the previous render-target resolution/AA infrastructure,
   with safe fallback and source/palette cache invalidation on level
   transitions; the first live-game OpenGL backend is now integrated.
3. Keep original motion-state/camera integration unchanged while improving
   FOV/pitch and modern input; horizontal and vertical live GPU mouse look
   are already implemented.
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
