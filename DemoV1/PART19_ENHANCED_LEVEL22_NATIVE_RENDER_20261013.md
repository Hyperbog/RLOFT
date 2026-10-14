# Part 19 — Enhanced PC native Level 22: decoded scenery and 60 Hz game display

**Policy:** [Enhanced PC Port Charter](ENHANCED_PC_PORT_CHARTER.md).  
**Branch:** `demo-v1`. **Release:** Pre-Alpha; native scene has not yet passed owner Windows playback acceptance.

## What changed (actual game code)

The player-facing cold-start sequence remains untouched:

`CINE00 → CINE12 → original logos → animated title → two-character CODE.1 creation → disk CHARS handoff → authored Level 22`.

The newly introduced **enhanced viewport** is in the same continuous native game window, not an external app/selector or a Level22 developer warp.

### 1. Decode and draw original RES3 world sprites directly

New `DemoV1/include/rloft/enhanced_type7_compositor.hpp` composes the **existing exact decoded EH type-7 RES3 pixel indices and transparency mask**, rather than requiring success from the DOS-specific compiled SINTAB VGA microcode tracer to show world objects.

- Retains the original current-frame `WorldSceneRenderPlan.visible_columns` and wall-depth clipped screen columns, plus existing draw-call order. Thus the original generated scene resources and world/NPC selection still control *which sprite is visible and where*; no replacement tree/assassin art is invented.
- Y projects source rows 0..127 using the shipped SINTAB scale block's `scale_step[y]` count, with native source row 64 anchored at viewport horizon y=132.
- Maps x=0..255 visibility columns to full-frame x=32..287 and clips every pixel to world viewport x=32..287, y=4..259. The opaque mask—not color 0—controls transparency, including palette index zero.
- Prevalidates source/mask lengths, source column bounds, framebuffer size and scale runs before indexed writes. Failed individual sources retain original resource index/error metadata; other sprites are not suppressed.
- Decodes and caches each RES3 identity once in `ExplorationPresenter`. The existing original VGA trace interpreter remains in the source for archival tests, but is **not required for player-facing native sprite pixels**. Real game uses the native path by default.
- Draw order remains: original RES8 sky/environment → RES4 ground → surface/wall material → RES3 trees/objects/NPCs → party HUD. Original `world_scene_render_plans` are *not* replaced with fictional sprites.

### 2. Render the original RES4 terrain behind foreground walls

`ExplorationPresenter::enhanced_ground_eligible` accepts a verified 4,098-byte source original RES4 texture and ready perspective table regardless of the old special `0x93` no-wall condition. The full lower 128-row original textured ground now fills the background; subsequent wall and sprite layers can occlude it normally.

This is an intentional enhanced-PC rendering change. It does not imply equality to the original DOS partial-depth `FF50` floor fill. The original `original_full_ground_eligible` analysis stays intact for reference and its former regression tests remain.

If RES4 cannot be decoded, the safe existing fallback remains and the original source-identification failure is retained; the renderer **does not invent grass pixels**.

### 3. Gameplay only: 60 Hz presentation, without touching previous animation pacing

Only the eight frame delay sites in `run_exploration` now use `kNativeGameplayRefreshHz = 60.0`. The existing CINEMATIC and CODE.1 frame delays remain on their earlier 70.0863 Hz / 54,925-microsecond-related schedules. Their encoded RES6 sound cues for lady/cat openings (frames 1, 25, 36), original voice, music sequence events, and card flipping code are not rewritten. Movement integration still uses its separately scheduled gameplay cadence; faster monitor paint frequency is not a movement speed change.

The established native GDI window already suppresses WM_ERASEBKGND and paints via an offscreen bitmap; this patch targets a more direct reason for the owner-reported empty/flickering field: scenery drawing used to fail closed when the DOS VGA trace was unsupported.

### 4. Stop executing the DOS VGA tracer during ordinary gameplay

After composing native sprites, the legacy runtime was still computing
`Type7VgaFrameTraceModel::try_build` for every scene each frame. This could
require additional original SINTAB generated-code and plane-mask inspection
even though the result was no longer used to draw native pixels.

- `ExplorationRuntime::set_enhanced_pc_scene_rendering(true)` turns off
  **only** the DOS VGA trace build. Scene selection, world records,
  authored draw calls and depth-clipped `WorldSceneRenderPlan` stay intact.
- Normal Windows `run_exploration` sets this mode when `--dev-trace` is
  **not** requested. The forensic trace remains available in developer
  diagnostics and native runtime source tests (the default constructor
  still retains its previous behavior).
- `enhanced_scene_mode_tests` checks this opt-in state.
  `enhanced_level22_player_scene_source_tests` now rejects twelve
  deliberately introduced regressions, including re-enabling the DOS
  trace on a player frame or failing to opt into the faster native path.

The enhanced scene-planning route also sets
`skip_dos_microcode=true` in `WorldSceneRenderPlanModel::try_build`,
which preserves original RES3 shape geometry, draw-call order, SINTAB
scale and wall-depth clipped columns **without** constructing unused
self-modifying-x86 patch descriptors. Strict `build()` and default
`try_build()` remain unchanged for forensic tests. An extended
`world_scene_render_plan_tests` regression verifies that deliberately
corrupt DOS-only patch offsets still leave the enhanced sprite visible
in the planning result, while the strict reference path rejects them.

This is a targeted reduction in CPU work per gameplay frame, not a claim
of a measured Windows FPS improvement. Such a measurement requires an
actual player device and completed source-matched build.

## Tests and limits

- `enhanced_type7_compositor_tests` (registered CTest): palette/mask, first and last viewport columns, horizon, 1x/2x/4x vertical scale, blank source rows, clipping, painter order, corrupt source and malformed scale fail-closed behavior.
- `exploration_presenter_tests`: full source-verified RES4 floor may be composited behind foreground geometry even for non-0x93 wall material, while missing/malformed RES4 still fails safely.
- `enhanced_level22_player_scene_source_tests` (registered Python CTest): source keeps original sky→ground→walls→sprites→HUD layering, native sprite default, 60 Hz gameplay with original CODE.1/CINE clocks intact, and eight deliberately broken pipeline/animation variants are rejected.
- The existing `alpha_frontend_preservation_contract_tests` pins six front-end C++ files to their previously working Git blobs. Changes here are in the world presenter and 60 Hz exploration loop only.
- A **local GCC C++17 synthetic, interface-equivalent compositor smoke test** passed `-Wall -Wextra -Wpedantic -Werror -O2 -DNDEBUG` using minimal resource/framebuffer test fixtures. This is **not** an exact GitHub checkout compiled or proof of Windows rendering.
- Original `SINTAB` file available to the project was inspected independently: it contains 127 scale blocks and 128 native row run lengths per block; their sums progress 4, 8, ... 508 destination rows. The native row scaler is founded on these real values.

## Unverified / required next field check

**Do not claim first playable native Level 22 is accepted yet.** Current exact-HEAD Windows CI/build status, first-frame render observations, animation sound/video capture, actual decoded tree/assassin visibility and frame pacing under Windows remain independently unproven. This task does not supply new proprietary game files.

Start the existing **`Ravenloft-Continuous-Alpha-Windows-x64`** artifact with owned original game files and capture one continuous cold start. Check the first Level 22 viewport for textured ground, sky, background, correct RES3 trees/assassin, coherent occlusion, no flicker, responsive movement, and unchanged intro/character audio and card direction. On failure, use the earlier opt-in `--dev-trace` diagnostic *separately*, not as the player-facing game.

**Next:** stop spending game-runtime critical path on original CODE.2 VGA instructions, verify the native sprite output visually, fix any remaining original asset/layout mistakes, then complete Level 22 interactions and port-native saves on the new enhanced-PC acceptance roadmap.
